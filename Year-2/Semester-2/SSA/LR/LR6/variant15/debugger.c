#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/personality.h>
#include <signal.h>
#include <inttypes.h>

static const char *signal_name(int sig)
{
    switch (sig) {
        case SIGTRAP: return "SIGTRAP";
        case SIGSEGV: return "SIGSEGV";
        case SIGILL:  return "SIGILL";
        case SIGFPE:  return "SIGFPE";
        case SIGBUS:  return "SIGBUS";
        case SIGINT:  return "SIGINT";
        case SIGTERM: return "SIGTERM";
        case SIGCHLD: return "SIGCHLD";
        case SIGCONT: return "SIGCONT";
        case SIGSTOP: return "SIGSTOP";
        default:      return "SIG?";
    }
}

static unsigned long find_symbol_addr(const char *binary, const char *symbol)
{
    char cmd[512];
    snprintf(cmd, sizeof cmd, "nm '%s' | awk '$3 == \"%s\" { print $1 }'", binary, symbol);
    FILE *p = popen(cmd, "r");
    if (!p) return 0;
    unsigned long addr = 0;
    if (fscanf(p, "%lx", &addr) != 1) addr = 0;
    pclose(p);
    return addr;
}

static long peek(pid_t pid, unsigned long addr)
{
    errno = 0;
    long word = ptrace(PTRACE_PEEKDATA, pid, (void *)addr, NULL);
    if (errno) {
        fprintf(stderr, "[dbg] PEEKDATA failed at 0x%lx: %s\n", addr, strerror(errno));
    }
    return word;
}

static void poke(pid_t pid, unsigned long addr, long word)
{
    if (ptrace(PTRACE_POKEDATA, pid, (void *)addr, (void *)word) < 0) {
        fprintf(stderr, "[dbg] POKEDATA failed at 0x%lx: %s\n", addr, strerror(errno));
    }
}

static int get_regs(pid_t pid, struct user_regs_struct *r)
{
    if (ptrace(PTRACE_GETREGS, pid, NULL, r) < 0) {
        fprintf(stderr, "[dbg] GETREGS failed: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

static int set_regs(pid_t pid, struct user_regs_struct *r)
{
    if (ptrace(PTRACE_SETREGS, pid, NULL, r) < 0) {
        fprintf(stderr, "[dbg] SETREGS failed: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

static void dump_regs(const struct user_regs_struct *r)
{
    printf("[dbg] registers: rip=0x%llx rsp=0x%llx rbp=0x%llx\n",
           (unsigned long long)r->rip,
           (unsigned long long)r->rsp,
           (unsigned long long)r->rbp);
    printf("[dbg]            rax=0x%llx rdi=0x%llx rsi=0x%llx rdx=0x%llx\n",
           (unsigned long long)r->rax,
           (unsigned long long)r->rdi,
           (unsigned long long)r->rsi,
           (unsigned long long)r->rdx);
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "usage: %s <target_binary> <symbol>\n", argv[0]);
        return 2;
    }
    const char *target = argv[1];
    const char *symbol = argv[2];

    unsigned long sym_addr = find_symbol_addr(target, symbol);
    if (!sym_addr) {
        fprintf(stderr, "[dbg] cannot find symbol '%s' in %s\n", symbol, target);
        return 1;
    }
    printf("[dbg] symbol '%s' at static address 0x%lx\n", symbol, sym_addr);

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) {
        /* child: become tracee, then exec the target */
        personality(ADDR_NO_RANDOMIZE);   /* disable ASLR for predictable addresses */
        if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0) {
            perror("TRACEME"); _exit(1);
        }
        execl(target, target, (char *)NULL);
        perror("execl");
        _exit(1);
    }

    /* parent: trace the child */
    int status;
    if (waitpid(pid, &status, 0) < 0) { perror("waitpid"); return 1; }
    if (!WIFSTOPPED(status)) {
        fprintf(stderr, "[dbg] child did not stop on exec\n");
        return 1;
    }
    printf("[dbg] child %d stopped on execve, installing breakpoint\n", pid);

    /* For a static (non-PIE) binary, the symbol's static address is the
       runtime address. Insert an INT3 (0xCC) at the symbol. */
    unsigned long bp_addr = sym_addr;
    long orig_word = peek(pid, bp_addr);
    long bp_word = (orig_word & ~0xffUL) | 0xccUL;
    poke(pid, bp_addr, bp_word);
    printf("[dbg] breakpoint armed at 0x%lx (orig byte=0x%02lx)\n",
           bp_addr, orig_word & 0xff);

    int hits = 0;
    while (1) {
        if (ptrace(PTRACE_CONT, pid, NULL, NULL) < 0) {
            perror("PTRACE_CONT");
            break;
        }
        if (waitpid(pid, &status, 0) < 0) { perror("waitpid"); break; }

        if (WIFEXITED(status)) {
            printf("[dbg] child exited normally, code=%d\n", WEXITSTATUS(status));
            break;
        }
        if (WIFSIGNALED(status)) {
            printf("[dbg] child killed by %s\n", signal_name(WTERMSIG(status)));
            break;
        }
        if (!WIFSTOPPED(status)) continue;

        int sig = WSTOPSIG(status);
        printf("[dbg] child stopped: signal=%s(%d)\n", signal_name(sig), sig);

        if (sig == SIGTRAP) {
            struct user_regs_struct r;
            if (get_regs(pid, &r) < 0) break;
            /* INT3 leaves rip pointing one byte past the trap instruction */
            r.rip -= 1;
            ++hits;
            printf("[dbg] hit breakpoint #%d at 0x%llx\n",
                   hits, (unsigned long long)r.rip);
            dump_regs(&r);

            /* Restore original byte so the instruction can execute */
            poke(pid, bp_addr, orig_word);
            if (set_regs(pid, &r) < 0) break;

            /* Single-step to execute the original instruction */
            if (ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL) < 0) {
                perror("SINGLESTEP"); break;
            }
            if (waitpid(pid, &status, 0) < 0) { perror("waitpid"); break; }
            if (!WIFSTOPPED(status)) break;

            /* Re-arm the breakpoint */
            poke(pid, bp_addr, bp_word);
            continue;
        }

        /* Forward other signals (e.g. SIGCHLD) to tracee */
        if (ptrace(PTRACE_CONT, pid, NULL, (void *)(long)sig) < 0) {
            perror("PTRACE_CONT (forward)");
            break;
        }
    }

    printf("[dbg] total breakpoint hits: %d\n", hits);
    return 0;
}

/*
 * Task 2.5 - Can we replace IP (Instruction Pointer) with stack top?
 *
 * This is a theoretical answer + demonstration program.
 *
 * Compile: gcc -Wall -o task2_5 ip_vs_stack.c
 */
#include <stdio.h>

/*
 * ANSWER TO TASK 2.5:
 *
 * NO, we cannot simply replace the Instruction Pointer (IP/PC) with
 * the top of the stack. Here's why:
 *
 * 1. DIFFERENT PURPOSES:
 *    - IP points to the NEXT instruction to execute (in code/text segment)
 *    - Stack top points to data (in stack segment)
 *    - They address completely different memory regions
 *
 * 2. EXECUTION FLOW:
 *    - IP advances sequentially through instructions
 *    - Stack grows/shrinks with push/pop operations
 *    - Using stack top as IP would mean every push/pop changes
 *      execution flow - this makes normal variable storage impossible
 *
 * 3. FUNCTION CALLS:
 *    - When calling a function: IP is saved on stack (return address),
 *      then IP jumps to function code
 *    - If we used stack top as IP, saving the return address would
 *      immediately redirect execution to the return address
 *    - This creates a paradox: you can't save the return address
 *      without triggering a return
 *
 * 4. INTERRUPTS:
 *    - Hardware interrupts save IP on stack and load new IP
 *    - If stack top = IP, any stack operation during interrupt
 *      handling would corrupt the execution flow
 *
 * 5. EXAMPLE:
 *    push eax     ; If stack top = IP, this would try to execute
 *                 ; the VALUE of eax as an instruction address!
 *
 * However, some stack-based architectures (like Forth machines) do
 * use a return stack where the top contains the return address,
 * but they STILL have a separate IP for sequential execution.
 */

/* Demonstration: show how IP and SP are different */

void func_c(void) {
    int local = 3;
    printf("func_c: stack var at %p\n", (void *)&local);
    printf("func_c: code at     %p\n", (void *)func_c);
    printf("  -> Stack and code are in completely different address spaces!\n");
}

void func_b(void) {
    int local = 2;
    printf("func_b: stack var at %p, code at %p\n",
           (void *)&local, (void *)func_b);
    func_c();
}

void func_a(void) {
    int local = 1;
    printf("func_a: stack var at %p, code at %p\n",
           (void *)&local, (void *)func_a);
    func_b();
}

int main() {
    int local = 0;

    printf("=== Task 2.5: IP vs Stack Top ===\n\n");
    printf("main: stack var at %p\n", (void *)&local);
    printf("main: code at     %p\n", (void *)main);
    printf("\nCall chain demonstration:\n");
    func_a();

    printf("\nConclusion: IP and stack pointer serve fundamentally\n");
    printf("different purposes and cannot be interchanged.\n");
    printf("IP -> code segment (sequential execution)\n");
    printf("SP -> stack segment (data storage)\n");

    return 0;
}

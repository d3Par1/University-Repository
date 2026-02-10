/*
 * Task 4.2: What happens with a negative argument to malloc?
 *
 * malloc takes size_t (unsigned). If we pass a negative signed value,
 * it gets implicitly converted to a very large unsigned number.
 * This can happen when integer overflow occurs in size calculations.
 *
 * Compile (64-bit): gcc -Wall -Wextra -std=c11 negative_malloc.c -o negative_malloc
 * Compile (32-bit): gcc -m32 -Wall -Wextra -std=c11 negative_malloc.c -o negative_malloc32
 * Run with ltrace: ltrace ./negative_malloc
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>

int main(void) {
    printf("=== Task 4.2: Negative argument to malloc ===\n\n");

    printf("sizeof(int)    = %zu\n", sizeof(int));
    printf("sizeof(size_t) = %zu\n\n", sizeof(size_t));

    /* Test 1: Direct negative value */
    printf("--- Test 1: malloc(-1) ---\n");
    errno = 0;
    void *p = malloc((size_t)-1);
    printf("  malloc((size_t)-1) = %p\n", p);
    printf("  (size_t)-1 = %zu\n", (size_t)-1);
    if (!p)
        printf("  errno = %d (%s)\n", errno, strerror(errno));
    free(p);

    /* Test 2: Integer overflow in multiplication */
    printf("\n--- Test 2: Overflow in xa * xb ---\n");
    int xa = 100000;
    int xb = 100000;

    /* Signed multiplication - overflow! */
    int num_signed = xa * xb;
    printf("  xa = %d, xb = %d\n", xa, xb);
    printf("  xa * xb (int) = %d (OVERFLOW!)\n", num_signed);
    printf("  Expected: %lld\n", (long long)xa * xb);

    /* If num_signed is negative due to overflow, malloc sees huge size */
    errno = 0;
    p = malloc((size_t)num_signed);
    printf("  malloc((size_t)%d) = malloc(%zu) => %p\n",
           num_signed, (size_t)num_signed, p);
    if (!p)
        printf("  errno = %d (%s)\n", errno, strerror(errno));
    free(p);

    /* Test 3: Safe multiplication with overflow check */
    printf("\n--- Test 3: Safe multiplication ---\n");
    size_t sa = (size_t)xa;
    size_t sb = (size_t)xb;

    /* Check for overflow before multiplication */
    if (sb != 0 && sa > SIZE_MAX / sb) {
        printf("  Overflow detected: %zu * %zu > SIZE_MAX\n", sa, sb);
    } else {
        size_t num_safe = sa * sb;
        printf("  Safe result: %zu * %zu = %zu\n", sa, sb, num_safe);
        p = malloc(num_safe);
        printf("  malloc(%zu) = %p\n", num_safe, p);
        free(p);
    }

    /* Test 4: Smaller values that overflow on 32-bit */
    printf("\n--- Test 4: Values that overflow on 32-bit ---\n");
    int a32 = 65536;
    int b32 = 65537;
    int result32 = a32 * b32;
    printf("  %d * %d = %d (int)\n", a32, b32, result32);
    printf("  Expected: %lld\n", (long long)a32 * b32);

    errno = 0;
    p = malloc((size_t)result32);
    printf("  malloc((size_t)%d) = %p\n", result32, p);
    if (!p)
        printf("  errno = %d (%s)\n", errno, strerror(errno));
    else
        printf("  (allocated, but with wrong size!)\n");
    free(p);

    /* Test 5: Negative sizes that look valid */
    printf("\n--- Test 5: Small negative -> large unsigned ---\n");
    int neg = -16;
    printf("  malloc((size_t)%d) = malloc(%zu)\n", neg, (size_t)neg);
    errno = 0;
    p = malloc((size_t)neg);
    printf("  Result: %p\n", p);
    if (!p)
        printf("  errno = %d (%s)\n", errno, strerror(errno));
    free(p);

    printf("\n=== Conclusion ===\n");
    printf("Passing a negative signed value to malloc is dangerous:\n");
    printf("  - The value is implicitly cast to size_t (unsigned)\n");
    printf("  - A small negative becomes a very large positive\n");
    printf("  - On 64-bit, this typically causes malloc to fail (NULL)\n");
    printf("  - On 32-bit, -1 = 4294967295, still fails\n");
    printf("  - Integer overflow in size calculation is a common bug\n");
    printf("  - Always use size_t for sizes and check for overflow\n");

    return 0;
}

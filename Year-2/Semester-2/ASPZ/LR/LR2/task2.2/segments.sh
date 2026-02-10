#!/bin/bash
# Task 2.2 - Explore ELF segments: text, data, BSS
# Run all steps and analyze segment sizes

echo "=== Task 2.2: ELF Segment Analysis ==="
echo

# Step 1: Basic hello world
echo "--- Step 1: Basic hello world ---"
cat > hello.c << 'EOF'
#include <stdio.h>
int main() { printf("Hello World\n"); return 0; }
EOF
gcc -Wall hello.c -o hello
echo "File size: $(ls -l hello | awk '{print $5}') bytes"
echo "Segments:"
size hello
echo

# Step 2: Add global array of 1000 int (uninitialized -> BSS)
echo "--- Step 2: Global int[1000] (uninitialized -> BSS) ---"
cat > hello2.c << 'EOF'
#include <stdio.h>
int big_array[1000];
int main() { printf("Hello World\n"); return 0; }
EOF
gcc -Wall hello2.c -o hello2
echo "File size: $(ls -l hello2 | awk '{print $5}') bytes"
echo "Segments:"
size hello2
echo

# Step 3: Initialize the array (BSS -> data)
echo "--- Step 3: Global int[1000] = {1} (initialized -> data) ---"
cat > hello3.c << 'EOF'
#include <stdio.h>
int big_array[1000] = {1};
int main() { printf("Hello World\n"); return 0; }
EOF
gcc -Wall hello3.c -o hello3
echo "File size: $(ls -l hello3 | awk '{print $5}') bytes"
echo "Segments:"
size hello3
echo

# Step 4: Local arrays (on stack, NOT in segments)
echo "--- Step 4: Local arrays (stack, not in ELF) ---"
cat > hello4.c << 'EOF'
#include <stdio.h>
int main() {
    int local_uninit[5000];
    int local_init[5000] = {1};
    printf("Hello World\n");
    (void)local_uninit; (void)local_init;
    return 0;
}
EOF
gcc -Wall hello4.c -o hello4
echo "File size: $(ls -l hello4 | awk '{print $5}') bytes"
echo "Segments:"
size hello4
echo

# Step 5: Debug vs optimized
echo "--- Step 5: Debug build ---"
gcc -g -O0 -Wall hello.c -o hello_dbg
echo "Debug file size: $(ls -l hello_dbg | awk '{print $5}') bytes"
echo "Debug segments:"
size hello_dbg
echo

echo "--- Step 5: Optimized build (-O3) ---"
gcc -O3 -Wall hello.c -o hello_opt
echo "Optimized file size: $(ls -l hello_opt | awk '{print $5}') bytes"
echo "Optimized segments:"
size hello_opt
echo

# Summary
echo "=== SUMMARY ==="
echo "File          | File Size | text    | data    | bss"
echo "---------------------------------------------------"
for f in hello hello2 hello3 hello4 hello_dbg hello_opt; do
    fsize=$(ls -l $f | awk '{print $5}')
    segments=$(size $f | tail -1 | awk '{printf "%7s | %7s | %7s", $1, $2, $3}')
    printf "%-13s | %7s | %s\n" "$f" "$fsize" "$segments"
done

echo
echo "Key observations:"
echo "  1. BSS segment grows with uninitialized globals (NO file size increase)"
echo "  2. Data segment grows with initialized globals (file size increases)"
echo "  3. Local variables do NOT appear in ELF segments (they use stack)"
echo "  4. Debug info increases file size but NOT text/data/bss"
echo "  5. Optimization primarily affects text segment size"

# Cleanup
rm -f hello.c hello2.c hello3.c hello4.c hello hello2 hello3 hello4 hello_dbg hello_opt

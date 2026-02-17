#!/bin/bash
# Завдання 1.26: Порівняння GCC та Clang

SRC="hello.c"
cat > $SRC << 'EOF'
#include <stdio.h>
#include <math.h>

int main() {
    double sum = 0;
    for (int i = 0; i < 10000000; i++) {
        sum += sin(i * 0.001) * cos(i * 0.002);
    }
    printf("Result: %f\n", sum);
    return 0;
}
EOF

echo "=== Task 1.26: GCC vs Clang Comparison ==="
echo

echo "--- Compiler Versions ---"
gcc --version | head -1
clang --version | head -1
echo

echo "--- Debug Build ---"
gcc -g -gdwarf-4 -O0 -Wall -Wextra $SRC -o hello_gcc_dbg -lm
clang -g -gdwarf-4 -O0 -Wall -Wextra $SRC -o hello_clang_dbg -lm

echo "GCC debug size:   $(wc -c < hello_gcc_dbg) bytes"
echo "Clang debug size: $(wc -c < hello_clang_dbg) bytes"
echo

echo "--- Disassembly (main function) ---"
objdump -d hello_gcc_dbg | grep -A 30 '<main>:' > gcc_disasm.txt
objdump -d hello_clang_dbg | grep -A 30 '<main>:' > clang_disasm.txt
echo "GCC disasm saved to gcc_disasm.txt"
echo "Clang disasm saved to clang_disasm.txt"
echo "Differences:"
diff gcc_disasm.txt clang_disasm.txt | head -20
echo

echo "--- Optimization Comparison ---"
for OPT in O0 O1 O2 O3 Os; do
    gcc -$OPT $SRC -o hello_gcc_$OPT -lm
    clang -$OPT $SRC -o hello_clang_$OPT -lm
    gcc_size=$(wc -c < hello_gcc_$OPT)
    clang_size=$(wc -c < hello_clang_$OPT)
    echo "  -$OPT: GCC=${gcc_size}B, Clang=${clang_size}B"
done
echo

echo "--- Performance (O2) ---"
echo "GCC -O2:"
time ./hello_gcc_O2
echo
echo "Clang -O2:"
time ./hello_clang_O2
echo

echo "--- Performance (O3) ---"
echo "GCC -O3:"
time ./hello_gcc_O3
echo
echo "Clang -O3:"
time ./hello_clang_O3

rm -f $SRC hello_gcc_* hello_clang_* gcc_disasm.txt clang_disasm.txt

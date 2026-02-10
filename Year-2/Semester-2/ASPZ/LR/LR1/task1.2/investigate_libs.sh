#!/bin/bash
# Task 1.2 - Library investigation script
# Analyzes libraries, finds math functions, checks symbols

echo "=== Task 1.2: Library Investigation ==="
echo

# 1. Check which libraries are used by common programs
echo "--- Libraries used by /bin/ls ---"
ldd /bin/ls
echo

echo "--- Libraries used by /usr/bin/gcc ---"
ldd $(which gcc) 2>/dev/null || echo "gcc not found or statically linked"
echo

# 2. Find math library
echo "--- Finding math library ---"
find /usr/lib -name "*libm*" 2>/dev/null
find /lib -name "*libm*" 2>/dev/null
echo

# 3. Check math library symbols for erf
echo "--- Symbols containing 'erf' in libm ---"
LIBM=$(find /usr/lib /lib -name "libm.so*" 2>/dev/null | head -1)
if [ -n "$LIBM" ]; then
    echo "Found libm at: $LIBM"
    nm -D "$LIBM" 2>/dev/null | grep erf || echo "Using objdump instead..."
    objdump -T "$LIBM" 2>/dev/null | grep erf
else
    echo "libm.so not found, trying libm-*.so"
    find /usr/lib /lib -name "libm*" -exec echo "Found: {}" \; 2>/dev/null
fi
echo

# 4. Analyze library dependencies
echo "--- Library dependencies (libm) ---"
if [ -n "$LIBM" ]; then
    ldd "$LIBM" 2>/dev/null || objdump -p "$LIBM" 2>/dev/null | grep NEEDED
fi
echo

# 5. Search for math functions across all libraries in /usr/lib
echo "--- Searching for sin, cos, exp in /usr/lib libraries ---"
echo "(This may take a moment...)"

SEARCH_FUNCS="sin\b|cos\b|exp\b"
found_count=0

for lib in /usr/lib/x86_64-linux-gnu/lib*.so* /usr/lib/lib*.so* 2>/dev/null; do
    if [ -f "$lib" ]; then
        result=$(nm -D "$lib" 2>/dev/null | grep -wE "sin|cos|exp" | head -5)
        if [ -n "$result" ]; then
            echo
            echo "  Library: $lib"
            echo "$result" | sed 's/^/    /'
            found_count=$((found_count + 1))
        fi
    fi
done

echo
echo "Found math functions in $found_count libraries."
echo
echo "=== Investigation Complete ==="

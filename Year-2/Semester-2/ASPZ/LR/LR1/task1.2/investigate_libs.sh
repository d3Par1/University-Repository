#!/bin/bash
# Завдання 1.2: Дослідження бібліотек

echo "=== Task 1.2: Library Investigation ==="
echo

echo "--- Libraries used by /bin/ls ---"
ldd /bin/ls
echo

echo "--- Libraries used by /usr/bin/gcc ---"
ldd $(which gcc) 2>/dev/null || echo "gcc not found or statically linked"
echo

echo "--- Finding math library ---"
find /usr/lib -name "*libm*" 2>/dev/null
find /lib -name "*libm*" 2>/dev/null
echo

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

echo "--- Library dependencies (libm) ---"
if [ -n "$LIBM" ]; then
    ldd "$LIBM" 2>/dev/null || objdump -p "$LIBM" 2>/dev/null | grep NEEDED
fi
echo

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

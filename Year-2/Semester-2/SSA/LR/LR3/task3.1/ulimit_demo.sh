#!/bin/bash
# Task 3.1: Experimenting with ulimit open files limit in Docker
# Run inside a Docker container: docker run -it ubuntu bash
#
# Usage: bash ulimit_demo.sh
# With root: sudo bash ulimit_demo.sh

echo "=== Task 3.1: ulimit open files experiment ==="
echo ""

echo "--- Step 1: Current open files limit ---"
ulimit -n

echo ""
echo "--- Step 2: Soft limit for open files ---"
ulimit -aS | grep "open files"

echo ""
echo "--- Step 3: Hard limit for open files ---"
ulimit -aH | grep "open files"

echo ""
echo "--- Step 4: Set soft limit to 3000 ---"
ulimit -n 3000
echo "Result: $?"

echo ""
echo "--- Step 5: Check soft limit after change ---"
ulimit -aS | grep "open files"

echo ""
echo "--- Step 6: Check hard limit after change ---"
ulimit -aH | grep "open files"

echo ""
echo "--- Step 7: Try to set limit to 3001 (above hard limit) ---"
ulimit -n 3001 2>&1
echo "Result: $?"

echo ""
echo "--- Step 8: Try to set limit to 2000 (lowering) ---"
ulimit -n 2000
echo "Result: $?"

echo ""
echo "--- Step 9: Current limit ---"
ulimit -n

echo ""
echo "--- Step 10: Soft limit now ---"
ulimit -aS | grep "open files"

echo ""
echo "--- Step 11: Hard limit now ---"
ulimit -aH | grep "open files"

echo ""
echo "--- Step 12: Try to set back to 3000 ---"
ulimit -n 3000 2>&1
echo "Result: $?"

echo ""
echo "=== Explanation ==="
echo "1. ulimit -n shows the current soft limit for open files."
echo "2. Soft limit can be raised up to the hard limit."
echo "3. Hard limit can only be raised by root."
echo "4. Once hard limit is lowered, non-root users cannot raise it back."
echo "5. Setting limit above hard limit fails for non-root users."
echo ""
echo "=== Root experiment ==="
echo "When running as root:"
echo "  - Both soft and hard limits can be freely adjusted"
echo "  - There is no restriction on raising the hard limit"
echo "  - The kernel's fs.nr_open parameter is the absolute maximum"

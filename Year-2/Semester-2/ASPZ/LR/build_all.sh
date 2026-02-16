#!/bin/bash
# Build all ASPZ Lab tasks in WSL Ubuntu
# Usage: bash build_all.sh [clean]

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

OK=0
FAIL=0
SKIP=0

report() {
    echo -e "${GREEN}=== Build Summary ===${NC}"
    echo -e "  ${GREEN}OK:   $OK${NC}"
    echo -e "  ${RED}FAIL: $FAIL${NC}"
    echo -e "  ${YELLOW}SKIP: $SKIP${NC}"
}

if [ "$1" = "clean" ]; then
    echo "Cleaning..."
    make -C "$SCRIPT_DIR/LR1" clean 2>/dev/null || true
    make -C "$SCRIPT_DIR/LR2" clean 2>/dev/null || true
    make -C "$SCRIPT_DIR/LR3" clean 2>/dev/null || true
    make -C "$SCRIPT_DIR/LR4" clean 2>/dev/null || true
    make -C "$SCRIPT_DIR/LR5" clean 2>/dev/null || true
    echo "Done."
    exit 0
fi

echo -e "${YELLOW}=== Building LR1 ===${NC}"

# --- LR1 individual compilations ---
build_single() {
    local dir="$1" src="$2" out="$3" flags="$4"
    printf "  %-30s" "$dir..."
    if gcc -Wall -Wextra -std=c11 -o "$SCRIPT_DIR/LR1/$dir/$out" "$SCRIPT_DIR/LR1/$dir/$src" $flags 2>/tmp/aspz_err; then
        echo -e " ${GREEN}OK${NC}"
        ((OK++))
    else
        echo -e " ${RED}FAIL${NC}"
        cat /tmp/aspz_err
        ((FAIL++))
    fi
}

build_multi() {
    local dir="$1" out="$2" flags="$3"
    shift 3
    local srcs=("$@")
    local full_srcs=()
    for s in "${srcs[@]}"; do
        full_srcs+=("$SCRIPT_DIR/LR1/$dir/$s")
    done
    printf "  %-30s" "$dir..."
    if gcc -Wall -Wextra -std=c11 -o "$SCRIPT_DIR/LR1/$dir/$out" "${full_srcs[@]}" $flags 2>/tmp/aspz_err; then
        echo -e " ${GREEN}OK${NC}"
        ((OK++))
    else
        echo -e " ${RED}FAIL${NC}"
        cat /tmp/aspz_err
        ((FAIL++))
    fi
}

# Task 1.1 (multi-file)
build_multi "task1.1" "task1_1" "-lm" "main.c" "erf_module.c"

# Task 1.2 (script only)
printf "  %-30s" "task1.2 (script)..."
echo -e " ${YELLOW}SKIP${NC}"
((SKIP++))

# Tasks 1.3, 1.4 have their own Makefiles
for t in task1.3 task1.4; do
    printf "  %-30s" "$t (make)..."
    if make -C "$SCRIPT_DIR/LR1/$t" -s 2>/tmp/aspz_err; then
        echo -e " ${GREEN}OK${NC}"
        ((OK++))
    else
        echo -e " ${RED}FAIL${NC}"
        cat /tmp/aspz_err
        ((FAIL++))
    fi
done

# Task 1.5
build_single "task1.5" "dependencies.c" "task1_5" ""

# Simple single-file tasks
build_single "task1.6"  "seconds_converter.c" "task1_6"  ""
build_single "task1.7"  "base_converter.c"    "task1_7"  "-lm"
build_single "task1.8"  "array_search.c"      "task1_8"  "-lm"
build_single "task1.9"  "string_replace.c"    "task1_9"  ""
build_single "task1.10" "tomorrow_date.c"     "task1_10" ""
build_single "task1.11" "student_db.c"        "task1_11" ""
build_single "task1.12" "generic_array.c"     "task1_12" ""
build_single "task1.13" "dynamic_array.c"     "task1_13" ""
build_single "task1.14" "last.c"              "last"     ""
build_single "task1.15" "sorting.c"           "task1_15" "-fopenmp"
build_single "task1.16" "struct_sort.c"       "task1_16" ""
build_single "task1.17" "insort.c"            "task1_17" ""
build_single "task1.18" "binary_print.c"      "task1_18" ""
build_single "task1.19" "char_freq.c"         "task1_19" ""
build_single "task1.20" "csv_table.c"         "task1_20" ""
build_single "task1.21" "equation_roots.c"    "task1_21" "-lm"
build_single "task1.22" "circular_buffer.c"   "task1_22" "-pthread"
build_single "task1.23" "bit_invert.c"        "task1_23" ""
build_single "task1.24" "swap_macro.c"        "task1_24" ""
build_single "task1.25" "random_float.c"      "task1_25" ""

# Task 1.26 (script only)
printf "  %-30s" "task1.26 (script)..."
echo -e " ${YELLOW}SKIP${NC}"
((SKIP++))

echo ""
echo -e "${YELLOW}=== Building LR2 ===${NC}"

build_single_lr2() {
    local dir="$1" src="$2" out="$3" flags="$4"
    printf "  %-30s" "$dir..."
    if gcc -Wall -Wextra -std=c11 -o "$SCRIPT_DIR/LR2/$dir/$out" "$SCRIPT_DIR/LR2/$dir/$src" $flags 2>/tmp/aspz_err; then
        echo -e " ${GREEN}OK${NC}"
        ((OK++))
    else
        echo -e " ${RED}FAIL${NC}"
        cat /tmp/aspz_err
        ((FAIL++))
    fi
}

build_single_lr2 "task2.1"   "time_end.c"        "time_end"        ""
build_single_lr2 "task2.2"   "fruts.c"            "fruts"           ""
build_single_lr2 "task2.3"   "stack_location.c"   "stack_location"  ""
build_single_lr2 "task2.4"   "stacker.c"          "stacker"         ""
build_single_lr2 "task2.5"   "ip_vs_stack.c"      "ip_vs_stack"     ""
build_single_lr2 "variant15" "segment_timing.c"   "segment_timing"  "-O0 -lrt"

# Script
printf "  %-30s" "task2.2 (script)..."
echo -e " ${YELLOW}SKIP${NC}"
((SKIP++))

echo ""
echo -e "${YELLOW}=== Building LR3 ===${NC}"

build_single_lr3() {
    local dir="$1" src="$2" out="$3" flags="$4"
    printf "  %-30s" "$dir..."
    if gcc -Wall -Wextra -std=c11 -o "$SCRIPT_DIR/LR3/$dir/$out" "$SCRIPT_DIR/LR3/$dir/$src" $flags 2>/tmp/aspz_err; then
        echo -e " ${GREEN}OK${NC}"
        ((OK++))
    else
        echo -e " ${RED}FAIL${NC}"
        cat /tmp/aspz_err
        ((FAIL++))
    fi
}

# Task 3.1 (script only)
printf "  %-30s" "task3.1 (script)..."
echo -e " ${YELLOW}SKIP${NC}"
((SKIP++))

build_single_lr3 "task3.2"   "perf_limit.c"    "perf_limit"    ""
build_single_lr3 "task3.3"   "dice_sim.c"       "dice_sim"      ""
build_single_lr3 "task3.4"   "lottery.c"         "lottery"       ""
build_single_lr3 "task3.5"   "filecopy.c"        "filecopy"      ""
build_single_lr3 "task3.6"   "stack_limit.c"     "stack_limit"   ""
build_single_lr3 "variant15" "ulimit_fsize.c"    "ulimit_fsize"  "-lrt"

echo ""
echo -e "${YELLOW}=== Building LR4 ===${NC}"

build_single_lr4() {
    local dir="$1" src="$2" out="$3" flags="$4"
    printf "  %-30s" "$dir..."
    if gcc -Wall -Wextra -std=c11 -o "$SCRIPT_DIR/LR4/$dir/$out" "$SCRIPT_DIR/LR4/$dir/$src" $flags 2>/tmp/aspz_err; then
        echo -e " ${GREEN}OK${NC}"
        ((OK++))
    else
        echo -e " ${RED}FAIL${NC}"
        cat /tmp/aspz_err
        ((FAIL++))
    fi
}

build_single_lr4 "task4.1"   "max_malloc.c"         "max_malloc"         ""
build_single_lr4 "task4.2"   "negative_malloc.c"     "negative_malloc"    ""
build_single_lr4 "task4.3"   "malloc_zero.c"         "malloc_zero"        ""
build_single_lr4 "task4.4"   "malloc_bug.c"          "malloc_bug"         "-g"
build_single_lr4 "task4.5"   "realloc_fail.c"        "realloc_fail"       "-D_GNU_SOURCE"
build_single_lr4 "task4.6"   "realloc_null_zero.c"   "realloc_null_zero"  ""
build_single_lr4 "task4.7"   "reallocarray_test.c"   "reallocarray_test"  "-D_GNU_SOURCE"
build_single_lr4 "variant15" "glibc_vs_musl.c"       "malloc_bench"       "-D_GNU_SOURCE -pthread -lrt"

echo ""
echo -e "${YELLOW}=== Building LR5 ===${NC}"

build_single_lr5() {
    local dir="$1" src="$2" out="$3" flags="$4"
    printf "  %-30s" "$dir..."
    if gcc -Wall -Wextra -std=c11 -o "$SCRIPT_DIR/LR5/$dir/$out" "$SCRIPT_DIR/LR5/$dir/$src" $flags 2>/tmp/aspz_err; then
        echo -e " ${GREEN}OK${NC}"
        ((OK++))
    else
        echo -e " ${RED}FAIL${NC}"
        cat /tmp/aspz_err
        ((FAIL++))
    fi
}

build_single_lr5 "task5.1"   "uninitialized.c"       "uninitialized"       "-g"
build_single_lr5 "task5.2"   "out_of_bounds.c"       "out_of_bounds"       "-g"
build_single_lr5 "task5.3"   "memory_leak.c"         "memory_leak"         "-g"
build_single_lr5 "task5.4"   "undefined_behavior.c"  "undefined_behavior"  "-g"
build_single_lr5 "task5.5"   "fragmentation.c"       "fragmentation"       "-g"
build_single_lr5 "task5.6"   "dangling_pointer.c"    "dangling_pointer"    "-g"
build_single_lr5 "task5.7"   "double_free.c"         "double_free"         "-g"
build_single_lr5 "task5.8"   "buffer_overflow.c"     "buffer_overflow"     "-g -fno-stack-protector"
build_single_lr5 "task5.9"   "use_after_free.c"      "use_after_free"      "-g"
build_single_lr5 "task5.10"  "memory_corruption.c"   "memory_corruption"   "-g"
build_single_lr5 "variant15" "cyclic_refs.c"         "cyclic_refs"         "-g"

echo ""
report

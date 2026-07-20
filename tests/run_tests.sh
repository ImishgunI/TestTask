#!/bin/bash

set -e

GREEN="\033[0;32m"
RED="\033[0;31m"
NC="\033[0m"

pass() {
    echo -e "${GREEN}[PASS]${NC} $1"
}

fail() {
    echo -e "${RED}[FAIL]${NC} $1"
    exit 1
}

cleanup() {
    rm -f *.log stdout.txt stderr.txt
}

cleanup

echo "===== Running integration tests ====="

########################################
# Test 1 - default level
########################################

./main test.log INFO < tests/input_default.txt >stdout.txt 2>stderr.txt

grep -q "\[INFO\] Hello world" test.log || fail "Default INFO message"

pass "Default level"

cleanup

########################################
# Test 2 - filtering
########################################

./main test.log INFO < tests/input_filter.txt >stdout.txt 2>stderr.txt

grep -q "\[INFO\] Info message" test.log || fail "INFO missing"

grep -q "\[ERROR\] Error message" test.log || fail "ERROR missing"

if grep -q "\[DEBUG\]" test.log; then
    fail "DEBUG should not be written"
fi

pass "Log filtering"

cleanup

########################################
# Test 3 - /set
########################################

./main test.log ERROR < tests/input_set.txt >stdout.txt 2>stderr.txt

grep -q "\[DEBUG\] Debug after set" test.log || fail "/set command"

pass "/set command"

cleanup

########################################
# Test 4 - invalid /set
########################################

./main test.log INFO < tests/input_invalid_set.txt >stdout.txt 2>stderr.txt

grep -q "Unknown log level" stderr.txt || fail "Invalid /set"

pass "Invalid /set"

cleanup

########################################
# Test 5 - empty message
########################################

./main test.log INFO < tests/input_empty.txt >stdout.txt 2>stderr.txt

grep -q "Empty message" stderr.txt || fail "Empty message"

pass "Empty message"

cleanup

########################################
# Test 6 - invalid startup arguments
########################################

if ./main test.log TEST >/dev/null 2>&1; then
    fail "Invalid startup level"
fi

pass "Invalid startup arguments"

########################################
# Test 7 - missing arguments
########################################

if ./main >/dev/null 2>&1; then
    fail "Missing arguments"
fi

pass "Missing arguments"

########################################
# Test 8 - Stress test
########################################

for i in $(seq 1 500)
do
    echo "DEBUG Message $i" >> tests/input_stress.txt
done

echo "/exit" >> tests/input_stress.txt

./main stress.log DEBUG < tests/input_stress.txt

COUNT=$(grep -c "DEBUG" stress.log)

if [ "$COUNT" -ne 500 ]; then
    fail "Stress test"
fi

pass "Stress test"

echo
echo -e "${GREEN}All integration tests passed.${NC}"

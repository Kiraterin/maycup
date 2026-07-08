#!/bin/bash

PASS=0
FAIL=0

for file in test/cases/*.in
do
    name=$(basename "$file" .in)
    ./bin/md2html "$file" > /tmp/result.out
    if diff -q "/tmp/result.out" "test/expected/$name.out"
    then
        echo "[PASS] $name"
        ((PASS++))  
    else
        echo "[FAIL] $name"
        ((FAIL++))
    fi
done

echo "$PASS passed, $FAIL failed"
exit $FAIL
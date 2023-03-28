#!/bin/sh

function on_passed {
    echo "  passed $1"
}
function on_fail {
    echo "  failed, check $1"
}

for FILE in `ls tests/*.in`
do
    mkdir -p tests
    BASE=$(basename $FILE .in)
    echo "Testing: " $FILE
    ./vm_riskxvii $FILE > "tests/$BASE.run" 
    cmp -s "tests/$BASE.out" "tests/$BASE.run" && on_passed "$BASE.run" || on_fail "$BASE.diff"


done

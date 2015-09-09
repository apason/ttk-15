#!/bin/bash

cd "$(dirname "$0")"

target="../../target/"
compiler=$target"ttc -c"
linker=$target"linker"
machine=$target"ttk-15"

red='\033[0;31m'
green='\033[0;32m'
NC='\033[0m'


echo " "
echo "TESTING GENERAL TESTS"

for test in $(cat test.cfg | cut -f1 -d' '); do

	expected="$(cat test.cfg | grep $test | cut -f2 -d' ')"
	isdir=$(ls -l units/ | grep $test | cut -c1)

	if [ $isdir  = "d" ]; then
		bash dirtest.sh $test $expected 
		continue
	fi

	cp "units/"$test $test
	$compiler $test > /dev/null

	if [ $? != 0 ]; then
                echo compiling failed in test $test
                exit -1
        fi

	$linker $(echo $test | cut -f1 -d'.')".o15" > /dev/null

	if [ $? != 0 ]; then
                echo linkin failed in test $test
                exit -1
        fi

	actual="$machine a.out.b15"

        if [ `$actual` != $expected ]; then
                echo -e test $test ${red}FAILED!${NC} expected: $expected actual `$actual`
        else
                echo -e test $test ${green}PASSED!${NC}
        fi

        rm *.b15
        rm *.o15
	rm *.k15
done


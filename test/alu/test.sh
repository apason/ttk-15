#!/bin/bash

cd "$(dirname "$0")"

target="../../target/"
compiler=$target"compiler"
linker=$target"linker"
machine=$target"ttk-15"

red='\033[0;31m'
green='\033[0;32m'
NC='\033[0m'

echo " "
echo "TESTING ARITHMETIC LOGICAL MACHINE INSTRUCTIONS"

for test in $(cat test.cfg | cut -f1 -d' '); do

	cat head.k15 units/$(echo $test).k15 tail.k15 > test.k15

	$compiler test.k15 > /dev/null

	if [ $? != 0 ]; then
		echo compiling failed in test $test
		exit -1
	fi
	$linker test.o15 > /dev/null

	if [ $? != 0 ]; then
		echo linkin failed in test $test
		exit -1
	fi

	expected="$machine a.out.b15"
	actual="$(cat test.cfg | grep $test | cut -f2 -d' ')"
	if [ `$expected` != $actual ]; then
		echo -e test $test ${red}FAILED!${NC} expected: $actual actual `$expected`
	else
		echo -e test $test ${green}PASSED!${NC}
	fi

	rm *.b15
	rm test.k15
	rm *.o15
done

echo " "

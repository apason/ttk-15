#!/bin/bash

cd "$(dirname "$0")"

target="../../target/"
compiler=$target"compiler"
linker=$target"linker"
machine=$target"ttk-15"

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
		echo test $test FAILED! expected: `$expected` actual $actual
	else
		echo test $test PASSED!
	fi

	rm *.b15
	rm test.k15
	rm *.o15
done


#!/bin/bash

cd "$(dirname "$0")"

target="../../target/"
compiler=$target"compiler"
linker=$target"linker"
machine=$target"ttk-15"
converter="../target/converter"

red='\033[0;31m'
green='\033[0;32m'
NC='\033[0m'

echo " "
echo "TESTING MACHINE MEMORY WITH TTK91 AND TTK15 FORMATS"


for test in $(cat test.cfg | cut -f1 -d' '); do

	isdir=$(ls -l units/ | grep $test | cut -c1)

	if [ ! $isdir = "d" ]; then
		continue
	fi
	
	dir="units/"$test
	$converter $dir/$test.b91
	
	if [ $? != 0 ]; then
		echo converting .b91 to .b15 failed in $test
		continue
	fi

	$compiler $dir/$test.k15 -o $test.o15
	
	if [ $? != 0 ]; then
		echo compiling in $test failed
		rm result.b15
		continue
	fi
	
	$linker $test.o15
	
	if [ $? != 0 ]; then
		echo linking in $test failed
		rm $test.o15
		rm result.b15
		continue
	fi
	why=$(cmp result.b15 a.out.b15)
	if [ $(echo $why | wc -w) == "0" ]; then
		echo -e test $test ${green}PASSED!${NC}
	else
		echo -e test $tes ${red}FAILED!${NC} : $why
	fi
	rm $test.o15
	rm result.b15
	rm a.out.b15

done

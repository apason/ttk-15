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
echo "COMPARING BINARIES OF TTK-91 AND TTK-15 FORMATS"


for fullpathtest in units/*; do
	
	test=${fullpathtest##*/}
	if [ ! -d $fullpathtest ]; then
		continue
	fi
	
	dir="units/"$test
	binary91=$(ls ${dir}/*.b91 | awk '{print $1;}')
	$converter $binary91 > /dev/null
	
	if [ $? != 0 ]; then
		echo converting $dir/$binery91 to .b15 failed in $test
		continue
	fi

	source15=$(ls ${dir}/*.k15 | awk '{print $1;}')
	$compiler $source15 -o a.out.o15 > /dev/null
	
	if [ $? != 0 ]; then
		echo compiling $source15 in $test failed
		rm result.b15
		continue
	fi
	
	$linker a.out.o15 > /dev/null
	
	if [ $? != 0 ]; then
		echo linking in $test failed
		rm a.out.o15
		rm result.b15
		continue
	fi
	why=$(cmp result.b15 a.out.b15)
	if [ $(echo $why | wc -w) == "0" ]; then
		echo -e TEST: $test ${green}PASSED!${NC}
	else
		echo -e TEST: $test ${red}FAILED!${NC} : $why
		exit -1
	fi
	rm a.out.o15
	rm result.b15
	rm a.out.b15

done

#!/bin/bash

cd "$(dirname "$0")"

target="../../target/"
compiler=$target"ttc -c"
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

	ttksource=$(ls ${dir}/*.k* | awk '{print $1;}')
	$compiler $ttksource 
	
	if [ $? != 0 ]; then
		echo compiling $ttksource in $test failed
		rm result.b15
		continue
	fi
	
        $linker $(ls ${dir}/*.o15) > /dev/null
	
	if [ $? != 0 ]; then
		echo linking in $test failed
                rm $(ls ${dir}/*.o15)
		rm result.b15
		continue
	fi
	why=$(cmp result.b15 a.out.b15 -i0:4)
	if [ $(echo $why | wc -w) == "0" ]; then
		echo -e TEST: $test ${green}PASSED!${NC}
	else
		echo -e TEST: $test ${red}FAILED!${NC} : $why
		exit -1
	fi
        rm $(ls ${dir}/*.o15)
	rm result.b15
	rm a.out.b15

done

#!/bin/bash

target="../../target/"
compiler=$target"ttc"
linker=$target"linker"
machine=$target"ttk-15"

red='\033[0;31m'
green='\033[0;32m'
NC='\033[0m'

rm -f *.b15
rm -f *.k15
rm -f *.o15

codedir="units/"$1
cp $codedir/*.k15 .

for file in $(ls | grep .k15); do

	$compiler $file > /dev/null 
	if [ $? != 0 ]; then
		echo "Error when compiling $file aborting.."
		exit -1
	fi
done

$linker ./*.o15 > /dev/null

if [ $? != 0 ]; then 
	echo "Linking error"
	exit -1
fi

actual="$machine a.out.b15"
if [ `$actual` != $2 ]; then
	echo -e test $1 ${red}FAILED!${NC} expected: $2 actual `$actual`
else
        echo -e test $1 ${green}PASSED!${NC}
fi

rm -f *.b15
rm -f *.k15
rm -f *.o15

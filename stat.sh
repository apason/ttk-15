#!/bin/bash
c_amount=$(cat $(find src/ -name *.c ) | wc -l)
h_amount=$(cat $(find src/ -name *.h ) | wc -l)
total_c=$(($c_amount+$h_amount))
echo $c_amount lines of .c files
echo $h_amount lines of .h files
echo total of $total_c lines of c code

doc_lines=0
for f in $(find doc*); do
	if [ ! -d $f ] ; then
		doc_lines=$((doc_lines+$(wc -l $f | cut -f 1 -d " ")))
	fi
done

echo $doc_lines of documentation

sh_lines=$(cat $(find . -name "*.sh" ) | wc -l)

echo $sh_lines of shell scripts

ttk_lines=$(($(cat $(find . -name *.k91) | wc -l)+$(cat $(find . -name *.k15) | wc -l)))

echo $ttk_lines of test files and examples

make_lines=$(cat $(find . -name makefile) | wc -l)

echo $make_lines of makefiles

echo sum of all files: $(($total_c+$doc_lines+$sh_lines+$ttk_lines+$make_lines))

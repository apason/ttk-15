#linker#

linker is for linking ttk-15 object modules (.o15) to ttk-15 executable (.b15)

##usage##

Just run linker with object modules as parameters:

./linker mod1.o15 mod2.o15 mod3.o15

Output file is named a.out.b15

Optionally one can use -o flag to determine output file:

./linker mod1.o15 mod2.o15 -o myprog

modules and output flag can be in any order

##notes##

 * One and only one of modules must contain label "main". That module will be
 linked first and program will start from code block (start of code block! not from main label!)
 of that module! (if there is only 1 module to link, main label is not necessary)

 * Because compiler will produce object files of same type regardless of source code
 type (k91 or k15) it is possible to write portion of the program in k91 and other in k15
 
 * See compilers documentation for more details!

#linker#

linker is for linking ttk-15 object modules (.o15) to ttk-15 executable (.b15)
See compilers doc folder for .o15 file format!

##usage##

Just run linker with object modules as parameters:

./linker mod1.o15 mod2.o15 mod3.o15

Output file is named a.out.b15

Optionally one can use -o flag to determine output file:

./linker mod1.o15 mod2.o15 -o myprog

modules and output flag can be in any order

##notes##

 * One and only one of modules must contain label "main". That module will be
linked first and program will start from code block of that module! (if there
is only 1 module to link, main label is not necessary)


 * Since ttk-15 object modules does not have internal labels (all labels are
 visible to all linked modules!) it is important to make sure that there is no
 externally used labels defined in several modules! When linker finds a label
 it first checks label list of that module. If its not declared it searches
 other modules. SO! Although all labels are "global" it is possible to define
 same label names in several modules as long as those labels are not externally
 used! e.g. There can be 2 modules with label "loop" or label "function" but if
 those labels are used in module that does contain it, error is caused.

 * See compilers documentation for more details!

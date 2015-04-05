#linker#

linker is for linking ttk-15 object modules (.o15) to ttk-15 executable (.b15)
See compilers doc folder for .o15 file format!

##usage##

Just run linker with object modules as parameters:

[apa@jalla]$ ./linker mod1.o15 mod2.o15 mod3.o15

##notes##

 * One and only one of modules MUST contain label "main". That module will be
linked first and program will start from code block of that module! If there
is several modules with main label, the behavior of linking is undefined!

 * Since ttk-15 object modules does not have internal labels (all labels are
 visible to all linked modules!) it is important to make sure that there is no
 externally used labels defined in several modules! When linker finds a label
 it first checks label list of that module. If its not declared it searches
 other modules. SO! Although all labels are "global" it is possible to define
 same label names in several modules as long as those labels are not externally
 used! e.g. There can be 2 modules with label "loop" or label "function" but if
 those labels are used in module that does contain it, behavior will be
 undefined!

 * See compilers documentation for more details!

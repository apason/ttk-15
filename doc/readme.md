#ttk-15 project#

ttk-15 is general register machine, fully (backward) compatible with ttk-91.
At this point, ttk-15 project consists of 3 parts.

 * machine
 * compiler
 * linker

##machine##

machine is emulator of our 32 bit register machine written in C.
It can run ttk-91 (titokones format) or ttk-15 binary files.

##compiler##

compiler is for compiling ttk-91 and ttk-15 source code to ttk-15 binary.
That means object modules compiled with our compiler can not be run in other
ttk-91 machines! (e.g. titokone) It creates .o15 object modules that contains
compiled instructions, data area and symbol table.

##linker##

linker links one or more .o15 object modules to executable .b15 file which can
be run in machine.



###authors###

Arttu Kilpinen - apason

Hiski Ruhanen  - praetoria

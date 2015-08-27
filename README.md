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

##differencies between ttk-91 and titokone##
 * Support of unlimited amount of object modules and linking. Object modules
 has import and export tables so this feature also enables global/local variables
 (labels) and makes programming easier.
 * Floating point unit and 16bit floats in machine instructions.
 Machine supports normal 32bit floats and all computation happens in 32bit.
 16bit floats from machine instructions are converted to 32bit before calculation.
 FPU has several new machine instructions. More details in machines and compilers documentation.
 * Speed. Because ttk-15 is written in C it is much faster than original titokone
 * text mode. It is possible to run ttk-15 without any graphical elements, so it
 can be used without X11. In that way computation is much faster.
 * ncurses debugger. As titokone includes spring? based user interface, the ttk-15
 has ncurses based debugger. It is easy and intuitive to use and it can show data
 in decimal, hexadecimal or binary format.
 * Few new flags to state register: halt flag (for power of the processor)
 and trap flag for indicating debug mode (used in debugger)
 * Some machine instructions (e.g. STORE and JUMP) does not work in same way in ttk-15
 and ttk-91. Nevertheless ttk-15 has functionality to slightly modify ttk-91 binaries
 so it is fully compatible with ttk-91s binaries (b91 can be run in ttk-15 but not
 vice versa) and source code (so any ttk-91 source will have the same end result in ttk-15)


##coming up##
ttk-15 is still in under development. We are planned at least following things to next versions:
 * Graphical user interface and debugger (gtk, qt or both)
 * Implementation of breakpoints to debugger so debugging will be much easier.
 * ttk-15 and ttk-91 has very inefficient state register. In any exception
 (owerflow, div of zero etc..) the corresponding bits in state register is modified.
 However since there is no instruction to investigate this register it is quite useless.
 In next versions there will be instruction for it so state register would have a real utility.
 * Implementation of hardware clock and real support for interrupts. This allows one to
 write strings or even operating system to ttk-15
 * Full windows support. We have not tested ttk-15 in windows but it might compile well
 under cygwin or mingw. Actually there is very little OS dependent code so porting would
 be very simple and fast.
 * linux man pages
 * pdf formatted users guide



For more information please read documentation in doc/ folder. We are willing to
help if issues araise. You can contact us from issues section on github or
directly to email. We are also grateful for any bug report or improvement idea.

###authors###

Arttu Kilpinen - apason
arttu.kilpinen@helsinki.fi

Hiski Ruhanen  - praetoria
hiski.rauhanen@helsinki.fi
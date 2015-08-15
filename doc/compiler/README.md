#compiler#

compiler is for compiling either ttk-91 (.k91) or ttk-15 (.k15) into
the ttk-15 object file format (.o15)

##usage##

Compiler works by calling tcc source.k15 -o object.o15
or just ./compiler source.k15

If you want to use ttk-91 syntax you should name your source
files <source file>.k91

It will then generate an object file with the name <source file>.o15

##syntax##

 * Ttk-15 syntax differs a bit with ttk-91 syntax, mainly when using
the store or jump instructions, for more info check out the ttk91-
compatibility section.

 * Instructions are of the following form:
 - [label] operation [register], [mode][address][(index register)]
or
 - [label] operation [register], [mode][register]
 
 parts inside the square brackets are optional, except for some
operations which require at least one or two operands.

And care must be taken, if you use a register in the place of the second
operand, you can't use an index register. If you want to know why,
check out the "other aliases" section.

##notes##

 * Label or variable names can not start with a number, otherwise
they will be considered as a number and non-digits are discarded.

 * One and only one of modules MUST contain label "main". That module
will be the main module.
It will be linked first and program will start from code block of that module!

 * If you use a label which is not defined in a module where you reference it.
The linker will look for it inside other modules in the linking stage of your program.

##ttk-91 compatibility##

 * When compiling .k91 source files the compiler is fully compatible
with the ttk-91 source code format.

 Compatibility is achieved by these aliases:
 - store r1,variable; is turned into store r1,=variable
 - jump r1,label; is turned into jump r1,=label
 - call r1,label; is turned into call r1,=label

##other aliases##

 * When you want to use two registers as operands, which isn't possible at
the machine level, the compiler transforms these instructions to suit the machine

 - For example add r1,r2 will become add r1,=0(r2) and
add r1,@r2 will become add r1,0(r2)

 - using the '=' indexing mode with a register is not allowed in the compiler as
there is no instruction for it

##floating point extension##

 * Now there is support for floating point variables in ttk-15,
    they are available by reserving memory for a variable using dc keyword.
    For example:
    - x dc 10.13
    - y dc 10^13

 * Whenever you use any non digit characters ['.', '^'] the compiler assumes the value to be a float

 * Floating point variables like other variables reserved with dc take up 32 bits in memory.

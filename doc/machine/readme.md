#ttk-15#

ttk-15 machine is for running ttk-15 or ttk-91(titokones format) binaries

##usage##

To run .b15 or .b91 binary file start the machin from CLI with following syntax:

./ttk-15 [options] [file] [options]

NOTE! File and options can be in any order!

NOTE! All options are optional so ./ttk-15 tito.b91 works just fine

###options###

-m mode , where mode is either "b91" or "b15" specifies mode of binary.
if there is no mode option, ttk-15 will define mode from file suffix.
if file suffix is not .b91 or .b15 and -m is not used, then machine
will use b15 as default.

NOTE! mode option has higher priority than filename suffix! (see examples)

-f file , where file is name of executable file. If -f flag is not used,
ttk-15 will use first non-option argument as binary file

-g causes ttk-15 to run in interactive (debugging) mode. see doc/machine/debugger
for more information.

-M memsize , where memsize is memory size reserved for machine. Size is
declared in machine words (32bits) default value is 512

###examples###

./ttk-15 -m b91 foobar
execute file "foobar" (treated as b91 file)

./ttk-15 -g foobar.b91
execute file "foobar" (treated as b91 file because of filename suffix)
in debugging mode.

./ttk-15 -f program.b15 -m b91
execute program.b15 as b91 file regardless of file suffix!
NOTE! this can be confusing. the program.b15 should be really b91 program!

./ttk-15 -M 1024 program.b91 -g
execute program.b91 in debugging mode with 1024 words of memory

At this point the machine has no graphical user interface. It prints its
output to stdout and reads (instruction IN) from stdin.

NOTE! When using -g with ncurses enabled, machine does print and read
from that ncurses window and stdio and stdout is not used

##specifications##
ttk-15 is 32bit general register machine. File src/include/ttk-15.h gives
good view about its structure. 

It has 8 registers of which first 6 are general work registers, 7th is stack
pointer and 8th is frame pointer.

###state register###

State register is inside of machines control unit. It is also 32 bit register
although only 11 most significant bits are used (at this point). Bits are from
most significant bit:

 * G - indicates that in last COMP operation first parameter had greater value
 * E - in last COMP operation the operands was equivalent
 * L - first operand were less than second
 * O - indicates aritmethical overflow
 * Z - indicates division by zero
 * U - unknown operation code
 * M - forbidden memory address
 * I - indicates interrupt
 * S - indicates supervisor call
 * p - machine is running in privileged (root) mode
 * D - interrupts disabled
 * H - halt program
 * T - debugging (trap) mode enabled

###instructions###

All machine instructions are 32bits long. Those consists of 5 fields:

 * operation code  8b  (opcode)
 * first operand   3b  (rj)
 * addressind mode 2b  (m)
 * index register  3b  (ri)
 * address field   16b (d)

opcode specifies which machine instruction is used and every instruction must
contain it! (a valid one)

rj is used in every instruction except NOP, JUMP, and jump operations
that use state registers GEL bits. In arithmetic logical instructions
rj is first (or only in case of NOT) operand and the result of operation is
stored to it. In jump instructions (excluding those specified above) rj:s value
is used to determine if jump occurs. In PUSH, POP, PUSHR, POPR, SVC, CALL and
EXIT operations, rj is used as stack pointer.

m ri and d jointly determines the second operand:

let addr = ri (value in that register) + d

If m == 0, value of second operand is addr.

if m == 1, value of second operand is mem[addr]

if m == 2, value of second operand is mem[mem[addr]]

NOTE! register r0 can not be used as index register! (when r0 is used as index
register, it behaves like theres 0 in it. (r0 is not zeroed, it just behaves
like that)

NOTE! In low level calculation of second operand ALLWAYS works like this!
See notes in compilers readme!

instructions are: (second column is value in hexadecimal)

 * NOP       00	  No Operation. Ignores all the rest instruction fields
 * STORE     01	  stores value in rj to mem[addr] See Notes in compilers readme!
 * LOAD      02	  loads value of addr to rj
 * IN        03	  reads value from addr to rj
 * OUT       04   prints value of rj to addr

ALU operations. Result of operation is stored to rj

 * ADD       11   rj + addr
 * SUB       12   rj - addr
 * MUL       13	  rj * addr
 * DIV       14	  rj / addr
 * MOD       15	  rj mod addr

logical bitwise operations:

 * AND       16	  ri and addr
 * OR        17	  ri or  addr
 * XOR       18	  ri xor addr
 * SHL       19	  ri shift bits to left addr times
 * SHR       1A	  ri shift bits to right addr times. (logical shift)
 * NOT       1B	  not ri
 * SHRA      1C	  shift bits to right addr times. (arithmetic shift)

jump and compare operations

 * COMP      1F	  compare r1 and addr. set one of GEL bits to 1
 * JUMP      20	  jump (set pc) to addr
 * JNEG      21	  jump to addr if ri < 0
 * JZER      22	  jump to addr if ri == 0
 * JPOS      23	  jump to addr if ri > 0
 * JNNEG     24	  jump to addr if ri >= 0
 * JNZER     25	  jump to addr if ri != 0
 * JNPOS     26	  jump to addr if ri <= 0
 * JLES      27	  jump to addr if L bit is set in state register
 * JEQU      28	  jump to addr if E bit is set
 * JGRE      29	  jump to addr if G bit is set
 * JNLES     2A	  jump to addr if L bit is not set
 * JNEQU     2B	  jump to addr if E bit is not set
 * JNGRE     2C	  jump to addr if G bit is nor set

other operations

 * CALL      31	  call sub routine in addr.
 * EXIT      32	  exits from subroutine
 * PUSH      33	  push addr to stack pointed by ri
 * POP       34	  pop value from stack pointed by ri to rj
 * PUSHR     35	  push all registers to stack pointed by ri
 * POPR      36	  pops all registers from stack pointed by ri
 * SVC       70	  call supervisor function addr. use ri as stack pointer

extended instructions: does same as original ones but with floating points

 * FIN       83   
 * FOUT      84
 * FADD      91
 * FSUB      92
 * FMUL      93
 * FDIV      94
 * FCOMP     9F
 * FJNEG     A1
 * FJZER     A2 
 * FJPOS     A3
 * FJNNEG    A4
 * FJNZER    A5 
 * FJNPOS    A6

FLOAD differs from LOAD only when using immediate addressing mode (mode 0)
when there is 16bit float in instruction. Otherwise it functions just as LOAD.
 * FLOAD     82


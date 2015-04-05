#ttk-15#

##usage##

To run .b15 binary file just start the machin from CLI and give it name of
executable file. e.g:

[apa@jalla]$ ./ttk-15 my_executable.b15

When running old .b91 file one must specify it with param -91. e.g:

[apa@jalla]$ ./ttk-15 -91 my_executable.b91


At this point the machine has no graphical user interface. It prints its
output to stdout and reads (instruction IN) from stdin.

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

NOTE: see compilers documentation!

 * CALL      31	  call sub routine in addr.
 * EXIT      32	  exits from subroutine
 * PUSH      33	  push addr to stack pointed by ri
 * POP       34	  pop value from stack pointed by ri to rj
 * PUSHR     35	  push all registers to stack pointed by ri
 * POPR      36	  pops all registers from stack pointed by ri
 * SVC       70	  call supervisor function addr. use ri as stack pointer


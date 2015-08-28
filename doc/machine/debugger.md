#Debugger#
TTK-15 machine includes ncurses based debugger. When enabled it interrupts execution 
between every machine instruction. To execute next instruction press enter anywhere
in debugger. 

##output format##
Output format means that all the values (in registers, memory and CRT) are shown in
a specific way. Available formats are Decimal (default), Hexadecimal and binary.
To switch to hexadecimal output format press 'x'. To switch binary press 'b' and to
switch decimal press 'd'. However indexes in MEM and CPU pages are allways in decimal.

##using it##

###CPU###
CPU page is the default starting window. It shows the content and the name of every machine
subunit and register. To enter this page press F1.

###MEM###
MEM page shows the content of machines memory. If all memory (512 words by default) does
not fit into one screen it only shows first memory slots. However one can investigate other
memory slots moving in the window:
home and end buttons: move to the first or last slots of the memory.
page up and page down: move to fixed amount of lines to up/down.
arrow keys up and down: move one line.
To enter this page press F2.

###CRT###
CRT page shows output of the machine. Unlike the MEM and CPU pages it only prints one output
per line. CRT page has the same movement commands as the MEM page. If no movement commands are
used the most recent output is shown in the last line. To enter this page press F3.

###The panel###
All pages mentioned above have a "control panel" in the first row. It indicates two different things:
Which page we are on and which output format is in use. It also gives instructions to change the view.

###input###
When the machine is waiting for input (IN instruction is executed) the debugger pops up a new input window.
The input window closes only when it is successfully read the input.
Input is always treated as a decimal number.
NOTE! As the machine is 32 bit, the input must also be at most 32 bit

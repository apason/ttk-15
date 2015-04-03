##format of object module##
#object module consists of 4 parts#
 * header
 * code block
 * data block
 * symbol table

#header#
header contains 2 integers 32 bit each. first integer defines starting point of symbol table. second 
integer defines starting point of data block.

#code block#
code block locates right after header (8 bytes from beginning).
It consists of 5 byte sub blocks. first byte of sub block defines mode for linker to operate 
instructions address. Value 0 means there is no label in instruction. linker will just copy this kind 
of instructions to executable. Value 1 means there is internal label. linker will copy particular 
label to address field. value 2 means external label. last 4 bytes of each subblock is instruction to 
copy (operated with labels as above)

#data block#
data block contains data of that module; Integers coded to 32 bit.

#symbol table#
symbol table consists of 34 byte sub blocks. each sub block contains 32 bytes reserved to label name 
and value of that label (16 bit integer ). label values are ralative to start of that block. external 
labels are valued from -1 to down.

##summary##

+--------------+    +-------------------------------+
| header       | -> | 32bit integer | 32bit integer |
+--------------+    +-------------------------------+
| code block   | -> +---------------------------------+
+--------------+    | 8bit marker | 32bit instruction |
| data block   |    +---------------------------------+
+--------------+    +-------------------------------+
| symbol table | -> | 32 char label | 16bit integer | 
+--------------+    +-------------------------------+

##Tests for ALU -instructions##

There are head.k15 and tail.k15 and units/ folder containing one line .k15 files. The test script 
creates test.k15 file concatenating head.k15 one file from unit folder and tail.k15. Then this 
file is compiled, linked and executed. There are test.cfg file containing row for every file in 
units/ folder. When one unit file is executed, the test script compares its output to data 
corresponding it in test.cfg.

#Restrictions and format#

 * Unit files should contain only one line of code! because the module size is a pre-decided: The 
code is 10 instructions and there is one labeled memoryslot. That slot contains number of its next 
memory slot which contains number 31. Also mem[31] contains a number 31.

Because of this value =x is evaluated to 10, x to 11 and @x to 31. =x(r3) is evaluated to 11, 
x(r3) to 31 and @x(r3) also to 31. At this moment there is tests for every alu operations in all 
modes described above. NOTE! values in indexed modes is same as values without indes thus those 
tests are irrelevanta (they only tests if the machine computes indexed values properly)

 * Script test.sh is not ideally written.. Line calculating expected value of output uses "grep 
<units file name> so there can not be several files so that one contains name of some other. e.g. 
there can not be unit files named "foo" and "foobar" cause grep foo returns both lines!

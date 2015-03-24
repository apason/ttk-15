CC= 	  gcc
STD=	  _GNU_SOURCE_
OBJS=	  main.o bitwise.o cu.o helpers.o instructions.o machine.o mmu.o loader.o
INCLUDE=  -I "../include/"
CFLAGS=   -Wall -D$(STD) $(INCLUDE) -c
MACHINE=  ./src/machine
COMPILER= ./src/compiler

.c.o:
	$(CC) $(CFLAGS) $<

all:	machine

machine:	$(OBJS)
		$(CC) $(OBJS) -o ../../target/ttk-15 

clean:
	rm -f *.o

clobber:	clean
	rm -f ttk-15

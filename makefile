MACHINE=  ./src/machine
COMPILER= ./src/compiler

.c.o:
	$(CC) $(CFLAGS) -I src/include $<

all:	machine

machine:	
	$(MAKE) -C $(MACHINE) all
clean:
	$(MAKE) -C $(MACHINE) clean

clobber:	clean
	$(MAKE) -C $(MACHINE) clobber

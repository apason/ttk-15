MACHINE=  ./src/machine
COMPILER= ./src/compiler

.c.o:
	$(CC) $(CFLAGS) -I src/include $<

all:	machine compiler

compiler:
	$(MAKE) -C $(COMPILER) all
machine:	
	$(MAKE) -C $(MACHINE) all
clean:
	$(MAKE) -C $(MACHINE) clean
	$(MAKE) -C $(COMPILER) clean

clobber:	clean
	$(MAKE) -C $(MACHINE) clobber
	$(MAKE) -C $(COMPILER) clobber

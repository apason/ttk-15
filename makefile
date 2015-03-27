MACHINE=  ./src/machine
COMPILER= ./src/compiler
LINKER=   ./src/linker

.c.o:
	$(CC) $(CFLAGS) -I src/include $<

all:	machine compiler linker

linker:
	$(MAKE) -C $(LINKER) all
compiler:
	$(MAKE) -C $(COMPILER) all
machine:	
	$(MAKE) -C $(MACHINE) all
clean:
	$(MAKE) -C $(MACHINE) clean
	$(MAKE) -C $(COMPILER) clean
	$(MAKE) -C $(LINKER) clean

clobber:	clean
	$(MAKE) -C $(MACHINE) clobber
	$(MAKE) -C $(COMPILER) clobber
	$(MAKE) -C $(LINKER) clobber

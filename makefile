MACHINE=  ./src/machine
COMPILER= ./src/compiler
LINKER=   ./src/linker
TEST=     ./test

build:	machine compiler linker

all:	machine compiler linker tests 

tests:	machine compiler linker src/include/*
	$(MAKE) -C $(TEST) all

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

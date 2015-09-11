## makefile for testing ##

.PHONY: alu general converter bitwise fpu
all:	alu general converter bitwise fpu

alu:
	bash alu/test.sh

bitwise:
	bash bitwise/test.sh

general: 
	bash general/test.sh
	
converter:
	$(MAKE) -C src/converter/ 
	bash ttk91vs15/test.sh
fpu:
	bash fpu/test.sh

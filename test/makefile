## makefile for testing ##

.PHONY: alu general converter bitwise
all:	converter alu general bitwise

alu:
	bash alu/test.sh

bitwise:
	bash bitwise/test.sh

general: 
	bash general/test.sh
	
converter:
	$(MAKE) -C src/converter/ 
	bash ttk91vs15/test.sh

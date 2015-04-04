## makefile for testing ##

.PHONY: alu1 general
all:	converter alu1 general 

alu1:
	bash alu1/test.sh

general: 
	bash general/test.sh
	
converter:
	$(MAKE) -C src/converter/ 

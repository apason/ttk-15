MACHINE=  ./src/machine
COMPILER= ./src/compiler
LINKER=   ./src/linker
TEST=     ./test

build:	machine compiler linker

all:	machine compiler linker tests build 

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

install:	build
	cp target/ttk-15 /usr/local/bin/ttk-15
	cp target/linker /usr/local/bin/linker
	cp target/ttc /usr/local/bin/ttc

remove:
	rm -f /usr/local/bin/ttk-15
	rm -f /usr/local/bin/linker
	rm -f /usr/local/bin/ttc



#this section is only for main developers and does not (propably) work with others

package: package_rpm package_deb

package_deb:	build
	cd packaging ; \
	./create_deb.sh $(version) ; \
	./install_deb.sh $(version) ;

#this assumes one have ~/rpmbuild and ~/.rpmmacros set up so building occurs in ones home direcrory
package_rpm:	build
	cd packaging ; \
	./create_rpm.sh $(version) ; \
	./install_rpm.sh $(version) ;


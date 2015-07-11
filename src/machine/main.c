#include <stdio.h>
#include <string.h>

//project headers
#include <ttk-15.h>
#include "machine.h"
#include "masks.h"

int main(int argc,char *argv[]){
    options *opts = getOptions(argc, argv);
    machine *m    = newMachine(opts->memsize);

    m->cu->pc = 0;

    if(opts == NULL){
	freeMachine(m);
	return -1;
    }

    if(opts->mode == TTK91)
	m->regs[6] = (MYTYPE) loadFile91(m->mem, opts->file, opts->memsize);
    else if(opts->mode == TTK15)
	m->regs[6] = (MYTYPE) loadFile(m->mem, opts->file, opts->memsize);

    //otherwise error occured
    if(m->regs[6] > 0){
	m->regs[7] = m->regs[6];

	if(opts->debug)
	    m->cu->sr |= TFLAG;

	//options are no longer needed
	freeOptions(opts);
	
	startMachine(m);
    }
    
    freeMachine(m);
    printf("\n");

    return 0;
}

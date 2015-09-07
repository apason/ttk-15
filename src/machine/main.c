#include <stdio.h>
#include <string.h>

//project headers
#include <ttk-15.h>
#include <module.h>
#include "machine.h"
#include "masks.h"

int main(int argc,char *argv[]){
    usage_list  *ul = NULL;
    options     *opts = getOptions(argc, argv);
    header_data *header;
    machine     *m;

    if(opts == NULL) return -1;

    m = newMachine(opts->memsize);
    m->cu->pc = 0;

    if(opts->mode == TTK91)
	m->regs[6] = (MYTYPE) loadFile91(m->mem, opts->file, opts->memsize);

    else if(opts->mode == TTK15){
	header = readHeader(opts->file);
	if(opts->memsize < header->usage_start -ftell(opts->file)) ;//error

	m->regs[6] = (MYTYPE) loadFile(m->mem, opts->file, opts->memsize);
	
	ul = readUsages(opts->file, header->usage_start);

	//debug
	/* printUsageList(ul); */
	/* return 0; */
    }

    //otherwise error occured
    if(m->regs[6] > 0){
	m->regs[7] = m->regs[6];

	if(opts->debug){
	    m->cu->sr |= TFLAG;
	    initScreen(header->pl, ul);
	}

	//options are no longer needed
	freeOptions(opts);

	startMachine(m);
    }
    
    freeMachine(m);
    printf("\n");

    return 0;
}

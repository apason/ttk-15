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
    int          limit;
    
    if(opts == NULL) return -1;

    m = newMachine(opts->memsize);
    m->cu->pc = 0;

    if(opts->mode == TTK91)
	m->regs[6] = (MYTYPE) loadFile91(m->mem, opts->file, opts->memsize);

    else if(opts->mode == TTK15){
	header = readHeader(opts->file);

	//printHeader(header);

	limit = (header->usage_start -ftell(opts->file)) / sizeof(MYTYPE);
	if(opts->memsize < limit) ;//ERROR!

	//if usage start is < 0, there is no usage table in binary
	m->regs[6] = (MYTYPE)loadFile(m->mem, opts->file, header->usage_start > 0 ? \
				      limit : opts->memsize);
	
	ul = readUsages(opts->file, header->usage_start);

	//debug prints addresses in bytes!
	//printUsageList(ul);

    }

    //otherwise error occured
    if(m->regs[6] > 0){
	m->regs[7] = m->regs[6];

	if(opts->debug){
	    int len = codeLength(header);
	    printf("construct condes\n"); fflush(NULL);
	    char **codes = constructCodes(header->pl, ul, len, m->mem);
	    printf("out\n"); fflush(NULL);
	    int i;
	    (void) i;
	    (void) codes;
	    (void) len;

	    /* for(i = 0; i < len; i++) */
	    /* 	printf("%s\n", codes[i]); */
	    
	    m->cu->sr |= TFLAG;
	    initScreen(codes, len, header->pl, &m->cu->pc);
	}

	//options are no longer needed
	freeOptions(opts);

	//	m->cu->sr &= ~TFLAG;
	startMachine(m);
    }
    
    freeMachine(m);
    printf("\n");

    return 0;
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <module.h>
#include <ttk-15.h>
#include "linker.h"
#include "error.h"


static char *getLabelName(llist *s, uint32_t instruction);
static int16_t findLabelValue(char *label);
static int findLabelAddressConstant(char *label);
static int handleExternalLabel(uint32_t *buf,  char *label, module *mod);
static int16_t findLocalLabel(char *label, llist *list);

//used in every function. initialized in linkmodule
static module **modules;
static int      n;


void linkModule(FILE *fp, module **modulestoinit, int mi, int m ){
    int i                            =  0;
    uint32_t codesize                = -1;
    uint32_t datasize                = -1;
    uint32_t buf                     =  0;
    char *label                      = NULL;
    
    module *mod                      = modulestoinit[mi];

    //initialize global functions
    modules = modulestoinit;
    n = m;

    codesize = (mod->data_start - CODESTART) / CODESIZE;
    datasize = (mod->export_start -mod->data_start) / sizeof(MYTYPE);

    //copy code segment to executable
    for(i = 0; i < codesize; i++){

	//contains next instruction
	memcpy(&buf, mod->codes[i] +1, sizeof(buf));

	switch(*(mod->codes[i])){
	//label from local module, without symboltable	    
	case LOCAL:
	case EXPORT:
	    buf += mod->address_constant / sizeof(MYTYPE);
	    break;

	//label from other module
	case IMPORT:
	    label = getLabelName(mod->import, buf);
	    if(label == NULL){
		errno_linker = EINVALID_IMPORT;
		printError(NULL,mod->filename);
		exit(-1);
	    }
	    if(handleExternalLabel(&buf, label, mod) != 0) exit(-1); //error. message already sent

	    break;
	    
	//addressing mode 0
	case NO_LABEL:
	    break;

        //this should not happen!
	default:
	    errno_linker = EINVALID_LABEL_TYPE;
	    printError(NULL,mod->filename);
	}
    


	/* if(label == NULL){ */
	/*     fprintf(stderr, "ERROR: Incorrect symbol table: nameless label with"); */
	/*     fprintf(stderr, " value %d. Aborting!\n", value); */
	/*     exit(-1); */
	/* } */
    

    fwrite(&buf, sizeof(buf), 1, fp);
    
	if(ferror(fp)){
	    fprintf(stderr, "Cant write to destination file.\n aborting!\n");
	    fclose(fp);
	    exit(-1);
	}
    }

    //copy data segment to executable ERROR endianess fucked up
    for(i = 0; i < datasize; i++)
	fwrite(mod->data +mod->data_start +i*sizeof(MYTYPE), sizeof(MYTYPE), 1, fp);

    freeRedundant(mod);

}

static int handleExternalLabel(uint32_t *buf, char *label, module *m){
    int16_t label_address_constant  = -1;
    
    *buf &= 0xFFFF0000;
    label_address_constant = findLabelAddressConstant( label);

    if(label_address_constant >= 0)
	*buf += label_address_constant +findLabelValue(label);
    else{
	errno_linker = label_address_constant;
	printError(NULL,m->filename, label);
	return -1;
    }

    return 0;
}

//find label address among all modules
static int findLabelAddressConstant(char *label){
    int    i  = 0;
    int    ac = ELABEL_NOT_FOUND;
    llist *s  = NULL;
  
    for(i = 0; i < n; i++)
	for(s = modules[i]->export; s; s = s->next)
	    if(!strncmp(s->label, label, LABELLENGTH)){
		if(ac == ELABEL_NOT_FOUND)
		    ac = modules[i]->address_constant / sizeof(MYTYPE);
		else
		    return EMULTIPLE_DECLARATIONS;
	    }
  
    return ac; 
}

//gets label name from modules own table
static char *getLabelName(llist *s, uint32_t instruction){
    int16_t label = 0;

    label = (int16_t) instruction;

    for(; s; s = s->next)
	if(s->value == label)
	    return s->label;
  
    return NULL;
}

//get label from list
static int16_t findLocalLabel(char *label, llist *list){ 
    for(; list; list = list->next)
	if(!strncmp(list->label, label, LABELLENGTH))
	   return list->value;
    return -1;
}

//get labels real value from arbitrary module
static int16_t findLabelValue(char *label){
    int  i   = 0;
    int  res = 0;

    if(label == NULL){
	printf("label name null! aborting..\n");
	exit(-1);
    }

    //no need to check multiple module definition
    for(i = 0; i < n; i++){
	res = findLocalLabel(label, modules[i]->export);
	if(res >= 0)
	    return res;
    }
    return -1;
}


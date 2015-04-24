#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "linker.h"
#include <module.h>
#include <ttk-15.h>

#define LABEL                   1
#define LABEL_NOT_FOUND        -2
#define MULTIPLE_DECLARATIONS  -1

static char *getLabelName(llist *s, uint32_t instruction);
static int16_t findLabelValue(char *label);
static int findLabelAddressConstant(char *label);
static void handleExternalLabel(int16_t value, uint32_t *buf,  char *label);

//used in every function. initialized in linkmodule
static module **modules;
static int      n;


void linkModule(FILE *fp, module **modulestoinit, int mi, int m ){
    int tmp                          =  0
    int i                            =  0;
    uint32_t codesize                = -1;
    uint32n_t datasize               = -1;
    uint32_t buf                     =  0;
    int16_t  value                   =  0;
    char *label                      = NULL;
    
    module *mod                      = modulestoinit[mi];

    //initialize global functions
    modules = modulestoinit;
    n = m;

    //filename to link
    printf("linking %s\n", modules[mi]->filename);

    codesize = (mod->data_start - CODESTART) / CODESIZE;
    datasize = (mod->symbol_start -mod->data_start) / sizeof(MYTYPE);

    //copy code segment to executable
    for(i = 0; i < codesize; i++){

	//contains next instruction
	memcpy(&buf, mod->codes[i] +1, sizeof(buf));

	switch(*(mod->codes[i])){
	//label from local module, without symboltable	    
	case LOCAL:
	    buf += mod->address_constant / sizeof(MYTYPE);
	    break;

	//label from other module
	case IMPORT:
	    label = getLabelName(mod->import, buf);
	    handleExternalLabel(&buf, label);
	    break;
	    
	//label from local symoltable
	case EXPORT:
	    label = GetLabelName(mod->export, buf);
	    tmp = findLocalLabel(label, mod->export);
	    if(tmp < 0) ;//fatal error
	    buf += tmp +mod->address_constant / sizeof(MYTYPE)
	    break;

	//addressing mode 0
	case NO_LABEL:
	    break;

        //this should not happen!
	default:
	    fprintf(stderr, "ERROR: incorrect module: unknown label type\n");
	    //error
	}
    


	if(label == NULL){
	    fprintf(stderr, "ERROR: Incorrect symbol table: nameless label with");
	    fprintf(stderr, " value %d. Aborting!\n", value);
	    exit(-1);
	}
    }

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

static void handleExternalLabel(uint32_t *buf, char *label){
    int16_t label_address_constant  = -1;
    int16_t value                   = (int16_t) *buf;
    
    *buf &= 0xFFFF0000;
    label_address_constant = findLabelAddressConstant( label);

    if(label_address_constant == LABEL_NOT_FOUND){
	fprintf(stderr, "ERROR: Undefined label %s. Aborting!\n", label);
	exit(-1);
    }

    if(label_address_constant == MULTIPLE_DECLARATIONS){
	fprintf(stderr, "ERROR: ambiguous label name %s. Aborting!\n", label);
	exit(-1);
    }
	
    *buf += label_address_constant +findLabelValue(label);
}

//find label address among all modules
static int findLabelAddressConstant(char *label){
    int    i  = 0;
    int    ac = LABEL_NOT_FOUND;
    llist *s  = NULL;
  
    for(i = 0; i < n; i++)
	for(s = modules[i]->export; s; s = s->next)
	    if(!strncmp(s->label, label, LABELLENGTH)){
		if(ac == LABEL_NOT_FOUND)
		    ac = modules[i]->address_constant / sizeof(MYTYPE);
		else
		    return MULTIPLE_DECLARATIONS;
	    }
  
    return ac; 
}

//gets label name from modules own table
static char *getLabelName(llist *s, uint32_t instruction){
    int16_t label = 0;

    if(s == NULL) printf("llist is null\n");

    label = (int16_t) instruction;

    for(; s; s = s->next)
	if(s->value == label)
	    return s->label;
  
    return NULL;
}

//get label from list
static int16_t findLocalLabel(char *label, llist *list){
    for(; list, list = list->next)
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


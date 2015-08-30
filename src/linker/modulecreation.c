#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

//project header
#include "linker.h"
#include "error.h"

#define SYMBOLSIZE 34  //size of one symbol sub block in .o15 format

static void readCode(module *mod);
static void readImportExport(module *mod);
static llist *readSymbols(module *mod, int start, int end);
static module *readModule(FILE *fp, char *filename);

//create all modules determined by argv
void createModules(int n, char **argv, module **modules){
    FILE *fp = NULL;
    int   i  = 0;
  
    for(i = 0; i < n; i++){

    
	fp = fopen(argv[i], "rb");
	if(fp == NULL){
	    perror("opening module");
	    exit(-1);
	}

	modules[i] = readModule(fp, argv[i]);

	if(modules[i] == NULL){
	    printError(NULL,argv[i]);
	    exit(-1);
	}

	fclose(fp);

	readCode(modules[i]);
	readImportExport(modules[i]);
    
    }
}

//initializes one module. returns NULL if any block is wrong size
static module *readModule(FILE *fp, char *filename){
    int i               = 0;
    uint32_t data_size  = -1;
    uint32_t code_size  = -1;
    module *mod         = (module *)malloc(sizeof(module));

    mod->filename = filename;

    //determine size
    fseek(fp, 0, SEEK_END);
    mod->size = ftell(fp);

    mod->data = (char *)malloc(mod->size);
    fseek(fp, 0, SEEK_SET);

    //read code and data starts
    fread(&mod->data_start, sizeof(MYTYPE), 1, fp);
    fread(&mod->export_start, sizeof(MYTYPE), 1, fp);
    fread(&mod->import_start, sizeof(MYTYPE), 1, fp);

    /*
     * module should allways be at least 16 bytes.
     * 
     * order of starts should be data <= export <= import
     *
     * x_start < size must be valid
     *
     * import and export table sizes should be a factor of labellength +2 = SYMBOLSIZE
     * code block should be factor of 5 and data block factor of 4
     */
        
    if((mod->data_start > mod->export_start)                           | \
       (mod->export_start > mod->import_start)                         | \
       (mod->import_start > mod->size )                                | \
       ((mod->import_start -mod->export_start) % SYMBOLSIZE != 0)      | \
       ((mod->size -mod->import_start) % SYMBOLSIZE != 0)              | \
       ((mod->data_start -CODESTART) % CODESIZE != 0)                  | \
       ((mod->export_start -mod->data_start) % sizeof(MYTYPE) != 0)){

	errno_linker = EINVALID_HEADER;
	return NULL;
    }

    if(mod->size < 16){
	errno_linker = ELITTLE_OBJECT;
	return NULL;
    }
    
    //calculate linked size
    code_size = sizeof(MYTYPE) * (mod->data_start - CODESTART) / CODESIZE;
    data_size = (mod->export_start -mod->data_start);
    mod->linked_size = code_size + data_size;

    fseek(fp, 0, SEEK_SET);
  
    //copy file to module
    for(i = 0; i < mod->size; i++)
	fread(mod->data +i, 1, 1, fp);

    return mod;
}

//constructs code array in module 
static void readCode(module *mod){
    int i    = 0;
    int size = (mod->data_start -CODESTART );
  
    mod->codes = (char **)malloc(sizeof(char *) * size);
  
    for(i = 0; i < size / CODESIZE; i++){
	mod->codes[i] = (char *)malloc(sizeof(char) * CODESIZE);
	memcpy(mod->codes[i], mod->data +(i * CODESIZE) +CODESTART, CODESIZE);
    }
  
}

//constructs symbol list in module
static void readImportExport(module *mod){
    mod->import = readSymbols(mod, mod->import_start, mod->size);
    mod->export = readSymbols(mod, mod->export_start, mod->import_start);

}

static llist *readSymbols(module *mod, int start, int end){
    int size = (end -start) / SYMBOLSIZE;
    int i    = 0;

    llist *list = (llist*) malloc(sizeof(llist));
    llist *head = list;

    //memset would be better?
    *(list->label +LABELLENGTH) = '\0';           //initialize
    list->next = NULL;                            //initialize

    for(i = 0; i < size; i++){
    
	strncpy(list->label, (char *)mod->data \
		+start +(i * SYMBOLSIZE), LABELLENGTH);
	list->value = *((int16_t *)(mod->data +start \
				      +(i * SYMBOLSIZE) +LABELLENGTH));

	if(i < size -1){
	    list->next = (llist *)malloc(sizeof(llist));
	    list = list->next;
	    list->next = NULL;
	}    
    }

    if(i == 0){
	free(head);
	return NULL;
    }
    
    return head;
}

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

//project header
#include "linker.h"

#define SYMBOLSIZE 34  //size of one symbol sub block in .o15 format

static void readCode(module *mod);
static void readImportExport(module *mod);
static void readSymbols(module *mod, llist *list, int start, int end);
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

	fclose(fp);

	readCode(modules[i]);
	readImportExport(modules[i]);
    
    }
}

//initializes one module
static module *readModule(FILE *fp, char *filename){
    printf("readModule"); fflush(NULL);
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
  
    //calculate linked size 
    code_size = sizeof(MYTYPE) * (mod->data_start - CODESTART) / CODESIZE;
    data_size = mod->import_start - mod->data_start;
    mod->linked_size = code_size + data_size;

    fseek(fp, 0, SEEK_SET);
  
    //copy file to module
    for(i = 0; i < mod->size; i++)
	fread(mod->data +i, 1, 1, fp);

    return mod;
}

//constructs code array in module 
static void readCode(module *mod){
    printf("readCode"); fflush(NULL);
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
    printf("readImportExport"); fflush(NULL);

    mod->import = (llist *)malloc(sizeof(llist));
    mod->export = (llist *)malloc(sizeof(llist));

    readSymbols(mod, mod->import, mod->import_start, mod->size);
    readSymbols(mod, mod->export, mod->export_start, mod->import_start);

}

static void readSymbols(module *mod, llist *list, int start, int end){
    printf("readSymbols\nstart: %d\nend %d\n",start,end); fflush(NULL);
    int size = (end -start) / SYMBOLSIZE;
    int i    = 0;

    //free memory!
    if((end -start) % SYMBOLSIZE != 0){
    	fprintf(stderr, "ERROR: incorrect module header!\n");
    	exit(-1);
    }

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
}





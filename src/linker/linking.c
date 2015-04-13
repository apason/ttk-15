#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "linker.h"
#include <ttk-15.h>

#define LABEL                   1
#define LABEL_NOT_FOUND        -2
#define MULTIPLE_DECLARATIONS  -1

static char *getLabelName(llist *s, uint32_t instruction);
static int16_t findLabelValue(module **modules, int n, char *label);
static int findLabelAddressConstant(module **modules, int n, char *label);

void linkModule(FILE *fp, module **modules, int mi, int n ){
  int i                            =  0;
  uint32_t codesize                = -1;
  uint32_t datasize                = -1;
  uint32_t buf                     =  0;
  int16_t  value                   =  0;
  int16_t  label_address_constant  = -1;
  char *label                      = NULL;
  module *mod                      = modules[mi];

  //filename to link
  printf("linking %s\n", modules[mi]->filename);

  codesize = (mod->data_start - CODESTART) / CODESIZE;
  datasize = (mod->symbol_start -mod->data_start) / sizeof(MYTYPE);

  //copy code segment to executable
  for(i = 0; i < codesize; i++){

    //contains next instruction
    memcpy(&buf, mod->codes[i] +1, sizeof(buf));
    
    //in this case  there is label in instruction
    if(*(mod->codes[i]) == 1){

      label = getLabelName(mod->symbols, buf);
      value = (int16_t) buf;

      if(label == NULL){
	fprintf(stderr, "ERROR: Incorrect symbol table: nameless label with");
	fprintf(stderr, " value %d. Aborting!\n", value);
	exit(-1);
      }

      //external label
      if(value < 0){
	buf &= 0xFFFF0000;
	label_address_constant = findLabelAddressConstant(modules, n, label);

	if(label_address_constant == LABEL_NOT_FOUND){
	  fprintf(stderr, "ERROR: Undefined label %s. Aborting!\n", label);
	  exit(-1);
	}

	if(label_address_constant == MULTIPLE_DECLARATIONS){
	  fprintf(stderr, "ERROR: ambiguous label name %s. Aborting!\n", label);
	  exit(-1);
	}
	
	buf += label_address_constant +findLabelValue(modules, n, label);

      }
      
      else buf += mod->address_constant / sizeof(MYTYPE);
      
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

//find label address among all modules
static int findLabelAddressConstant(module **modules, int n, char *label){
  int    i  = 0;
  int    ac = LABEL_NOT_FOUND;
  llist *s  = NULL;
  
  for(i = 0; i < n; i++)
    for(s = modules[i]->symbols; s; s = s->next)
      if(!strncmp(s->label, label, LABELLENGTH) && s->value >= 0){
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

//get labels real value from arbitrary module
static int16_t findLabelValue(module **modules, int n, char *label){
  int    i = 0;
  llist *s = NULL;

  if(label == NULL){
    printf("label name null! aborting..\n");
    exit(-1);
  }

  for(i = 0; i < n; i++)
    for(s = modules[i]->symbols; s; s = s->next)
      if(!strncmp(s->label, label, LABELLENGTH) && s->value >= 0)
	return s->value;
  
  return -1;
}


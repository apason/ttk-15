#include <stdio.h>
#include <string.h>

#include <linker.h>
#include <ttk-15.h>

#define LABEL 1

static char *getLabelName(llist *s, uint32_t label);
static int findLabelValue(module **modules, int n, char *label);
static int findLabelAddressConstant(module **modules, int n, char *label); 

void link(FILE *fp, module **modules, int mi, int n ){
  int i;
  uint32_t size, buf;
  int16_t value;
  char *label;
  module *mod     = modules[mi];

  size = (mod->data_start - CODESTART) / CODESIZE;

  //copy code segment to executable
  for(i = 0; i < size; i++){
    
    memcpy(&buf, mod->codes[i] +1, sizeof(buf));
    
    if(*(mod->codes[i]) == 1){

      label = getLabelName(mod->symbols, buf);
      value = findLabelValue(modules, n, label);

      //external label
      if(value > 0)
	buf += findLabelAddressConstant(modules, n, label) + value;
      //internal label
      else
	buf += mod->address_constant;
      
    }
    
    fwrite(&buf, sizeof(buf), 1, fp);
    
  }

  //copy data segment to executable
  for(i = 0; i < (mod->symbol_start - mod->data_start); i++)
    fwrite(mod->data +mod->data_start +i, sizeof(int8_t), 1, fp);

  freeRedundant(mod);
}

//find label address among all modules
static int findLabelAddressConstant(module **modules, int n, char *label){
  int i;
  llist *s;
  for(i = 0; i < n; i++)
    for(s = modules[i]->symbols; s; s = s->next)
      if(!strncmp(s->label, label, 32))
	return modules[i]->address_constant;
  return -1; //fatal error
}

//gets label name from modules own table
static char *getLabelName(llist *s, uint32_t label){
  for(; s; s = s->next)
    if(s->value == label)
      return s->label;
  return NULL;
}

//get labels real value from arbitrary module
static int findLabelValue(module **modules, int n, char *label){
  int i;
  llist *s;
  for(i = 0; i < n; i++)
    for(s = modules[i]->symbols; s; s = s->next)
      if(!strncmp(s->label, label, 32))
	return s->value;
  return 0xFFFF; //to be change
}

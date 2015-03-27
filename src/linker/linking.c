#include <stdio.h>
#include <string.h>

#include "linker.h"

static char *getLabel(llist *s, uint32_t label);
static int findLabelValue(module **modules, int n, char *label);

void link(FILE *fp, module **modules, int mi, int n ){
  int i;
  uint32_t buf;
  module *mod     = modules[mi];
  static int pos  = 0;

  //in this case mod is already linked
  if(mod->data == NULL)
    return;

  //copy code segment to executable
  for(i = 0; i < (mod->data_start - 8) / 5; i++){   //8 = code start
    memcpy(&buf, mod->codes[i] +1, 4);
    if(*(mod->codes[i]) == 1)
      buf += pos;
    if(*(mod->codes[i]) == 2)
      //ERROR! module constant of label should also be added!!!
      buf += pos + findLabelValue(modules, n, getLabel(mod->symbols, buf));
    fwrite(&buf, sizeof(buf), 1, fp);
  }

  //copy data segment to executable
  for(i = 0; i < (mod->symbol_start - mod->data_start); i++)
    fwrite(mod->data, sizeof(int8_t), 1, fp);
}

//gets label "value" from modules own table
static char *getLabel(llist *s, uint32_t label){
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

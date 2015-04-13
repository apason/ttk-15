#include <stdlib.h>

#include "linker.h"

static void freeSymbolList(llist *l);
static void freeModule(module *mod);

//free redundant data of module (codes and data already linked)
void freeRedundant(module *mod){
  int i = 0;
  int size = (mod->data_start - CODESTART) / 5;
  
  for(i = 0; i < size; i++)
    free(mod->codes[i]);
  
  free(mod->codes);
  free(mod->data);

  mod->codes = NULL;
  mod->data  = NULL;
}

//free module
static void freeModule(module *mod){
  if(mod->codes) free(mod->codes);
  if(mod->data)  free(mod->data);
  
  freeSymbolList(mod->symbols);
  free(mod);
}

static void freeSymbolList(llist *l){
  llist *li   = NULL;
  llist *prev = NULL;

  if(!l) return;
  
  prev = l;
  
  for(li = prev->next; li; li = li->next){
    free(prev);
    prev = li;
  }

  free(prev);
}

void freeModules(module **modules, int n){
  int     i   = 0;

  for(i = 0; i < n; i++)
    freeModule(modules[i]);
  
}

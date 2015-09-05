#include <stdlib.h>

#include "linker.h"

static void freeSymbolList(label_list *l);
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
  
  freeSymbolList(mod->import);
  freeSymbolList(mod->export);
  free(mod);
}

static void freeSymbolList(label_list *l){
  label_list *li   = NULL;
  label_list *prev = NULL;

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

  free(modules);
}

void freeOptions(options *opts){
    fclose(opts->output);
    free(opts);
}

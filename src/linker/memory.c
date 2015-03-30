#include <stdlib.h>

#include <linker.h>

static void freeSymbolList(llist *l);

//free redundant data of module (codes and data already linked)
void freeRedundant(module *mod){
  int i, size = (mod->data_start - CODESTART) / 5;
  for(i = 0; i < size; i++)
    free(mod->codes[i]);
  free(mod->codes);
  free(mod->data);
}

//free module
void freeModule(module *mod){
  freeSymbolList(mod->symbols);
  free(mod);
}

static void freeSymbolList(llist *l){
  llist *li, *prev;

  if(!l) return;
  
  prev = l;
  
  for(li = prev->next; li; li = li->next){
    free(prev);
    prev = li;
  }

  free(prev);
}

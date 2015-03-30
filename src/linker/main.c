#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <compiler.h>
#include <linker.h>

static int findMain(module **modules, int n);
static int containsMain(module *mod);
//static void printModule(module *mod);
//static void printSymbols(llist *l);

int main(int argc, char **argv){
  module **modules = (module **) malloc(sizeof(module *) * (argc -1));
  module *tmp;
  FILE *output;
  int i, mainnbr;


  //create module structs for each module
  printf("creating modules..\n");
  createModules(argc -1, argv +1, modules);

  //determine which module contains main
  printf("finding main module..\t");
  if((mainnbr = findMain(modules, argc -1)) < 0){
    printf("error: no main module found!\n");
    exit(-1);
  }
  
  printf("ok!\n");

  //swap main to first position
  if(mainnbr != 0){
    tmp = modules[0];
    modules[0] = modules[mainnbr];
    modules[mainnbr] = tmp;
  }

  //main module
  modules[0]->address_constant = 0;

  for(i = 1; i < argc -1; i++)
    modules[i]->address_constant = modules[i -1]->linked_size \
                                   +modules[i -1]->address_constant;

  output = fopen("a.out.b15", "wb");

  if(!output){
    perror("fopen");
    exit(-1);
  }

  printf("linking modules..\n");
  
  //link main containing module first
  link(output, modules, 0, argc -1);

  //link others
  for(i = 1; i < argc -1; i++) link(output, modules, i, argc -1);

  //free all modules
  for(i = 0; i < argc -1; i++) freeModule(modules[i]);

  fclose(output);

  return 0;
}

//determine main module (use first main containing module! change later)
static int findMain(module **modules, int n){
  int i;

  for(i = 0; i < n; i++)
    if(containsMain(modules[i]))
      return i;
  
  return -1;
}

//returns 1 if main label found
static int containsMain(module *mod){
  llist *li;
  
  for(li = mod->symbols; li != NULL; li = li->next)
    if(!strncmp(li->label, "main", 32))
      return 1;
  
  return 0;  
}

/*
static void printModule(module *mod){
  printf("nsize: %d\tdstart: %d\tsstart %d\n", mod->size, mod->data_start, mod->symbol_start);
  if(!mod->symbols)
    printf("symbol list = NULL\n");
  else
    printSymbols(mod->symbols);

}

static void printSymbols(llist *l){
  for(;l;l = l->next)
    printf("%s\t%d\n",l->label,l->value);
}
*/

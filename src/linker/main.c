#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

//project headers
#include <compiler.h>
#include <linker.h>

//used as return value of findMain
#define NO_MAIN       -2
#define SEVERAL_MAINS -1

static int findMain(module **modules, int n);
static int containsMain(module *mod);
//static void printModule(module *mod);
//static void printSymbols(llist *l);

int main(int argc, char **argv){
  int    mainnbr    = NO_MAIN;     
  int    i          = 0;
  FILE   *output    = NULL;        
  module **modules  = NULL;
  module *tmp       = NULL;

  modules  = (module **) malloc(sizeof(module *) * (argc -1));

  //create module structs for each module
  printf("creating modules..\n");
  createModules(argc -1, argv +1, modules);

  //determine which module contains main
  printf("finding main module..\t");
  if((mainnbr = findMain(modules, argc -1)) == NO_MAIN){
    fprintf(stderr, "error: no main module found!\n");
    freeModules(modules, argc -1);
    exit(-1);
  }

  if(mainnbr == SEVERAL_MAINS){
    fprintf(stderr, "error: several main modules!\n");
    freeModules(modules, argc -1);
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

  if(output == NULL){
    freeModules(modules, argc -1);
    perror("fopen");
    exit(-1);
  }

  //  for(i = 0; i < argc -1; i++)
  //  printModule(modules[i]);
  
  printf("linking modules..\n");
  
  //link main containing module first
  link(output, modules, 0, argc -1);

  //link others
  for(i = 1; i < argc -1; i++) link(output, modules, i, argc -1);

  freeModules(modules, argc -1);
  fclose(output);

  return 0;
}

//determine main module
static int findMain(module **modules, int n){
  int i = 0;
  int mainnbr = NO_MAIN;

  for(i = 0; i < n; i++)
    if(containsMain(modules[i])){
      if(mainnbr == NO_MAIN)
	mainnbr = i;
      else
	return SEVERAL_MAINS;
    }
  
  return mainnbr;
}

//returns 1 if main label is found
static int containsMain(module *mod){
  llist *li;
  
  for(li = mod->symbols; li != NULL; li = li->next)
    if(!strncmp(li->label, "main", 32))
      return 1;
  
  return 0;  
}

/*
static void printModule(module *mod){
  printf("size: %d\tlinksize %d\tdstart: %d\tsstart %d\tconstant %d\n", mod->size, mod->linked_size, mod->data_start, mod->symbol_start, mod->address_constant);
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

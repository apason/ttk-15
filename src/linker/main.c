#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <compiler.h>
#include "linker.h"

static int findMain(module **modules, int n);
static int containsMain(module *mod);

int main(int argc, char **argv){
  module **modules = (module**)malloc(sizeof(module*) * (argc -1));
  FILE *output;
  int i, mainnbr;

printf("Creating modules\n");
  //create module structs for each module
  createModules(argc -1, argv +1, modules);

printf("Finding main...\n");
  //determine which module contains main
  mainnbr = findMain(modules, argc -1);
  
 
  if((output = fopen("a.out.b15", "wb")) == NULL){
    printf("ERROR: cannot open result file\n");
    exit(-1);
  }

  //link main containing module to first one
  link(output, modules, mainnbr, argc -1);

  //means "module already linked"
  modules[mainnbr]->data = NULL; //free memory first!

  //link others
  for(i = 0; i < argc -1; i++)
    link(output, modules, i, argc -1);

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

static int containsMain(module *mod){
  llist *li;
  for(li = mod->symbols; li != NULL; li = li->next)
    if(!strncmp(li->label, "main", 32))
      return 1;
  return 0;
    
}

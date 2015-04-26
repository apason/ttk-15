#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

//project headers
#include "linker.h"

//used as return value of findMain
#define NO_MAIN       -2
#define SEVERAL_MAINS -1

static int findMain(module **modules, int n);
static int containsMain(module *mod);
static void printModule(module *mod);
static void printSymbols(llist *l);
static void setMain(module **mods, options *opts);

int main(int argc, char **argv){
    int      i        = 0;
    module **modules  = NULL;
    options *opts     = NULL;

    opts    = getOptions(argc, argv);
    modules = (module **) malloc(sizeof(module *) * (opts->count));

    if(opts->count < 1){
	printf("no modules specified!\n");
	exit(-1);
    }

    //create module structs for each module
    printf("creating modules..\n");
    createModules(opts->count, argv +1, modules);


    printModule(modules[0
			]);
    
    //if there is only 1 object module to link, skip main finding
    if(opts->count > 1) setMain(modules, opts);

    printf("ok!\n");
  
    //main module
    modules[0]->address_constant = 0;

    //set address constants to each module
    for(i = 1; i < opts->count; i++)
	modules[i]->address_constant = modules[i -1]->linked_size \
	    +modules[i -1]->address_constant;

    printf("linking modules..\n");
  
    //link main containing module first
    linkModule(opts->output, modules, 0, opts->count);

    //link others
    for(i = 1; i < opts->count; i++)
	linkModule(opts->output, modules, i, opts->count);

    freeModules(modules, opts->count);
    fclose(opts->output);

    return 0;
}

static void setMain(module **modules, options *opts){

    module *tmp     = NULL;
    int     mainnbr = 0;

    //determine which module contains main
    printf("finding main module..\t");
    if((mainnbr = findMain(modules, opts->count)) == NO_MAIN){
	fprintf(stderr, "error: no main module found!\n");
	freeModules(modules, opts->count);
	freeOptions(opts);
	exit(-1);
    }

    if(mainnbr == SEVERAL_MAINS){
	fprintf(stderr, "error: several main modules!\n");
	freeModules(modules, opts->count);
	freeOptions(opts);
	exit(-1);
    }
  
    //swap main to first position
    if(mainnbr != 0){
	tmp = modules[0];
	modules[0] = modules[mainnbr];
	modules[mainnbr] = tmp;
    }   
  
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
  
    for(li = mod->export; li != NULL; li = li->next)
	if(!strncmp(li->label, "main", 32))
	    return 1;
  
    return 0;  
}

//atm print only export table
static void printModule(module *mod){
    printf("size: %d\tlinksize %d\tdstart: %d\tistart %d\nestart %d\tconstant %d\n", mod->size, mod->linked_size, mod->data_start, mod->import_start, mod->export_start, mod->address_constant);
    if(!mod->import)
	printf("import list = NULL\n");
    else
	printSymbols(mod->import);
    if(!mod->export)
	printf("export list = NULL\n");
    else
	printSymbols(mod->export);

}

static void printSymbols(llist *l){
    for(;l;l = l->next)
	printf("%s\t%d\n",l->label,l->value);
}


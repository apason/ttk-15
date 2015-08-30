#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

//project headers
#include "linker.h"
#include "error.h"

static int findMain(module **modules, int n);
static int containsMain(module *mod);
static void printModule(module *mod);
static void printSymbols(llist *l);
static int setMain(module **mods, options *opts);

int main(int argc, char **argv){
    int      i        = 0;
    module **modules  = NULL;
    options *opts     = NULL;

    opts    = getOptions(argc, argv);
    modules = (module **) malloc(sizeof(module *) * (opts->count));

    if(opts->count < 1){
	errno_linker = ENO_MODULES;
	printError(NULL,NULL);
	//memory?
	exit(-1);
    }

    //create module structs for each module
    createModules(opts->count, argv +1, modules);

    //if there is only 1 object module to link, skip main finding
    if(opts->count > 1)
	if(setMain(modules, opts) != 0){
	    printError(NULL,NULL);
	    //memoryfree + exit
	}

    //main module
    modules[0]->address_constant = 0;

    //set address constants to each module
    for(i = 1; i < opts->count; i++)
	modules[i]->address_constant = modules[i -1]->linked_size \
	    +modules[i -1]->address_constant;

    /* for(i=0;i<opts->count;i++) */
    /* 	printModule(modules[i]); */


    //link main containing module first
    linkModule(opts->output, modules, 0, opts->count);

    //link others
    for(i = 1; i < opts->count; i++)
	linkModule(opts->output, modules, i, opts->count);

    
    freeModules(modules, opts->count);
    fclose(opts->output);
    free(opts);

    return 0;
}

static int setMain(module **modules, options *opts){

    module *tmp     = NULL;
    int     mainnbr = 0;

    //determine which module contains main
    mainnbr = findMain(modules, opts->count);

    if(mainnbr == ENO_MAIN){
	errno_linker = ENO_MAIN;
	return -1;
    }

    if(mainnbr == ESEVERAL_MAINS){
	errno_linker = ESEVERAL_MAINS;
	return -1;

    }
  
    //swap main to first position
    if(mainnbr != 0){
	tmp = modules[0];
	modules[0] = modules[mainnbr];
	modules[mainnbr] = tmp;
    }
    
    return 0;
}

//determine main module
static int findMain(module **modules, int n){
    int i = 0;
    int mainnbr = ENO_MAIN;

    for(i = 0; i < n; i++)
	if(containsMain(modules[i])){
	    if(mainnbr == ENO_MAIN)
		mainnbr = i;
	    else
		return ESEVERAL_MAINS;
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


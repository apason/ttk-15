#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

//project headers
#include <module.h>
#include "linker.h"
#include "error.h"

static int findMain(module **modules, int n);
static int containsMain(module *mod);
static void printModule(module *mod);
static void printSymbols(label_list *l);
static int setMain(module **mods, options *opts);
static void writeHeader(int count, module **modules, FILE *fp, int debug);

int main(int argc, char **argv){
    int         i        = 0;
    module    **modules  = NULL;
    options    *opts     = NULL;
    usage_list *list  = NULL;

    opts    = getOptions(argc, argv);
    modules = (module **) malloc(sizeof(module *) * (opts->count));

    if(opts->count < 1){
	errno_linker = ENO_MODULES;
	printError(NULL,NULL);
	//memory?
	exit(-1);
    }

    //create module structs for each module
    createModules(opts->count, argv +1, modules, opts->debug);

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


    //adjust label values in usage table
    if(opts->debug)
	for(i = 1; i < opts->count; i++){
	    list = modules[i]->usages;
	    for(; list; list = list->next)
		/*
		 * Label usage structure uses WORDS in values
		 * of addresses but module structure uses BYTES
		 * so we need to convert the values here!
		 */
		list->value += modules[i]->address_constant / sizeof(MYTYPE);
	}

    writeHeader(opts->count, modules, opts->output, opts->debug);

    //link main containing module first
    linkModule(opts->output, modules, 0, opts->count);

    //link others
    for(i = 1; i < opts->count; i++)
	linkModule(opts->output, modules, i, opts->count);

    if(opts->debug)
	for(i = 0; i < opts->count; i++){
	    list = modules[i]->usages;
	    for(; list; list = list->next){
		fwrite(&list->value, sizeof(MYTYPE), 1, opts->output);
		fwrite(&list->label, LABELLENGTH, 1, opts->output);
	    }
	}
    
    freeModules(modules, opts->count);
    fclose(opts->output);
    free(opts);

    return 0;
}

/*
 * NOTE! We need to use bytes here because sizes in the module struct
 * is also in bytes
 */
static void writeHeader(int count, module **modules, FILE *fp, int debug){
    MYTYPE header_size;
    MYTYPE codesize;
    MYTYPE tmp;
    int i;

    if(debug == 0){
	module *tmpmod = modules[count -1];
	//size of code area in WORDS
	int code_size = (tmpmod->data_start -CODESTART) / 5;

	//last code line in BYTES!
	printf("%d", code_size * 4 + tmpmod->address_constant);
	tmp = -(code_size * 4 + tmpmod->address_constant);
	fwrite(&tmp, sizeof(MYTYPE), 1, fp);
	return;
    }

    header_size = (1 + 2 * count) * sizeof(MYTYPE);

    for(i = 0; i <= count -1; i++){
	tmp = modules[i]->address_constant +header_size;
	fwrite(&tmp, sizeof(MYTYPE), 1, fp);

	codesize = (modules[i]->data_start - CODESTART) / CODESIZE;
	
	tmp = modules[i]->address_constant +codesize * sizeof(MYTYPE) +header_size;
	fwrite(&tmp, sizeof(MYTYPE), 1, fp);
    }

    //export start is offset in module! does not work
    tmp = modules[i -1]->address_constant +modules[i -1]->linked_size +header_size;
    fwrite(&tmp, sizeof(MYTYPE), 1, fp);
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
    label_list *li;
  
    for(li = mod->export; li != NULL; li = li->next)
	if(!strncmp(li->label, "main", 32))
	    return 1;
  
    return 0;  
}

//atm print only export table
static void printModule(module *mod){
    usage_list *ul;
    (void) ul;
    printf("size: %d\tlinksize %d\tdstart: %d\testart %d\nistart %d\tconstant %d\t%d ustart\n", mod->size, mod->linked_size, mod->data_start, mod->export_start, mod->import_start, mod->address_constant, mod->label_usage_start);
    if(!mod->import)
	printf("import list = NULL\n");
    else
	printSymbols(mod->import);
    if(!mod->export)
	printf("export list = NULL\n");
    else
	printSymbols(mod->export);
    if(!mod->usages)
	printf("usage list = NULL\n");
    for(ul = mod->usages;ul; ul = ul->next)
    	printf("%s\t%d\n",ul->label,ul->value);

}

static void printSymbols(label_list *l){
    for(;l;l = l->next)
	printf("%s\t%d\n",l->label,l->value);
}


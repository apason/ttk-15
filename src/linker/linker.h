#ifndef LINKER
#define LINKER

#include <stdio.h>
#include <stdint.h>

#include <ttk-15.h>
#include <module.h>

#define CODESIZE 5           //code sub block contains mode-byte and machine instructin
#define CODESTART 16          //starts from 16:th byte by .o15 module definition
#define USAGESIZE 36

typedef struct LABEL_LIST{
    struct LABEL_LIST *next;
    char          label[33];
    int16_t       value;
  
} label_list;

typedef struct MODULE {
    int16_t     address_constant;  //size in bytes!
    int         size;              //size in bytes!
    int         linked_size;       //size in bytes!
    int         data_start;        //size in bytes!
    int         import_start;      //size in bytes!
    int         export_start;      //size in bytes!
    int         label_usage_start;
    label_list *import;
    label_list *export;
    usage_list *usages; 
    char      **codes;
    char       *data;
    char       *filename;

} module;

typedef struct OPTIONS {
    FILE    *output;
    int      count;
    int      debug;
  
} options;

extern void linkModule(FILE *fp, module **modulestoint, int mi, int m );
extern void createModules(int n, char **argv, module **modules, int debug);
extern void freeRedundant(module *mod);
extern void freeModules(module **mod, int n);
extern options *getOptions(int argc, char **argv);
extern void freeOptions(options *opts);

#endif

#ifndef LINKER
#define LINKER

#include <stdio.h>
#include <stdint.h>

#include <ttk-15.h>

#define CODESIZE 5           //code sub block contains mode-byte and machine instructin
#define CODESTART 8          //starts from 8:th byte by .o15 module definition

typedef struct LLIST{
  struct LLIST *next;
  char          label[33];
  int16_t       value;
  
} llist;

typedef struct MODULE {
  int16_t address_constant;  //size in bytes!
  int     size;                 
  int     linked_size;
  int     data_start;
  int     symbol_start;
  llist  *symbols;
  char  **codes;
  char   *data;
  char   *filename;

} module;

typedef struct OPTIONS {
  FILE    *output;
  int      count;
  
} options;

extern void linkModule(FILE *fp, module **modules, int mi, int n );
extern void createModules(int n, char **argv, module **modules);
extern void freeRedundant(module *mod);
extern void freeModules(module **mod, int n);
extern options *getOptions(int argc, char **argv);

#endif

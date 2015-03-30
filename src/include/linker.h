#ifndef LINKER
#define LINKER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <ttk-15.h>

#define CODESIZE 5     //code sub block contains mode-byte and machine instructin
#define CODESTART 8    //starts from 8:th byte by .o15 module definition
#define LABELLENGTH 32 //max textual length of label

typedef struct LLIST{
  struct LLIST *next;
  char label[33];
  int16_t value;
} llist;

typedef struct MODULE {
  int16_t address_constant;
  int size;
  int linked_size;
  int data_start;
  int symbol_start;
  llist *symbols;
  char **codes;
  char *data;
  char *filename;
} module;

extern void link(FILE *fp, module **modules, int mi, int n );
extern void createModules(int n, char **argv, module **modules);
extern void freeModule(module *mod);
extern void freeRedundant(module *mod);

#endif

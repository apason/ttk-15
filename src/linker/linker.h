#ifndef LINKER
#define LINKER

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct LLIST{
  struct LLIST *next;
  char label[33];
  int value;
} llist;

typedef struct MODULE {
  int size;
  // int code_start; 8 by definition
  int data_start;
  int symbol_start;
  llist *symbols;
  char **codes;
  int8_t *data;
} module;

extern void link(FILE *fp, module **modules, int mi, int n );
extern void createModules(int n, char **argv, module **modules);

#endif

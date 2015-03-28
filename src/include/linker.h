#ifndef LINKER
#define LINKER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct LLIST{
  struct LLIST *next;
  char label[33];
  int value;
} llist;

typedef struct MODULE {
  int address_constant;
  int size;
  int linked_size;
  int data_start;
  int symbol_start;
  llist *symbols;
  char **codes;
  int8_t *data;
} module;

extern void link(FILE *fp, module **modules, int mi, int n );
extern void createModules(int n, char **argv, module **modules);

#endif

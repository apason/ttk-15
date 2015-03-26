#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <compiler.h>

#define LABELMASK 0xFF
#define CODESIZE 5
#define SYMBOLLINE

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

char *getLabel(llist *s, uint32_t label);
void link(FILE *fp, module **modules, int i, int n );
void readCode(module *m);
void readSymbols(module *m);
module *readModule(FILE *fp);


int main(int argc, char **argv){
  module **modules = (module**)malloc(sizeof(module*) * (argc -1));
  FILE *fp, *output;
  int i;
  module *module;

  //load filedescriptors
  for(i = 1; i < argc; i++){
    
    //open files
    if((fp = fopen(argv[1], "rb")) == NULL){
      printf("ERROR: cannot open file: %s\n", argv[i]);
      exit(-1);
    }

    modules[i -1] = readModule(fp);

    fclose(fp);

    readCode(modules[i -1]);
    readSymbols(modules[i -1]);
    
  }

  //determine main module (use first main containing module! change later)
  module = modules[0];
  for(i = 0; i < argc -1; i++)
    if(containsMain(modules[i]))
      break;

  if((output = fopen("a.out.b15", "bw")) == NULL){
    printf("ERROR: cannot open result file\n");
    exit(-1);
  }

  link(output, modules, i, argc -1);

  modules[i]->data = NULL; //free memory first!

  for(i = 0; i < argc -1; i++)
    link(output, modules, i, argc -1);

  fclose(output);

  return 0;
}

void link(FILE *fp, module **modules, int i, int n ){
  int j;
  static int pos = 0;
  module *mod = modules[i];

  if(mod->data=NULL)
    return;
  
  uint32_t buf;

  for(j = 0; j < (mod->data_start - 8 /*by definition*/) / 5; j++){
    memcpy(&buf, mod->codes[j] +1, 4 /*sizeof(MYTYPE)*/);
    if(*(mod->codes[j] +1) == 1)
      buf += pos;
    if(*(mod->codes[j] +1) == 2)
      buf += pos + findLabelValue(modules, n, getLabel(mod->symbols, buf && LABELMASK));
    fwrite(&buf, sizeof(buf), 1, fp);
  }
  
  for(j = 0; j < (mod->symbol_start - mod->data_start); j++)
    fwrite(mod->data, sizeof(int8_t), 1, fp);
}

char *getLabel(llist *s, uint32_t label){
  for(; s; s = s->next)
    if(s->value == label)
      return s->label;
}

int findLabelValue(module **modules, int n, char *label){
  int i;
  llist *s;
  for(i = 0; i < n; i++)
    for(s = modules[i]->symbols; s; s = s->next)
      if(!strncmp(s->label, label, 32))
	return s->value;
  return 0xFFFF; //to be change
}

int containsMain(module *mod){
  llist *li;
  for(li = mod->symbols; li != NULL; li = li->next)
    if(!strncmp(li->label, "main", 32))
      return 1;
  return 0;
    
}

void readCode(module *m){
  int i;
  int size = (m->data_start -8 /*code_start by definition*/);
  m->codes = (char**)malloc(size);
  for(i = 0; i < size / CODESIZE; i++){
    m->codes[i] = (char*)malloc(CODESIZE *sizeof(char));
    memcpy(m->codes[i], m->data +i +8, CODESIZE);
  }
}

void readSymbols(module *m){
  m->symbols = (llist*)malloc(sizeof(llist));
  llist *symbol=m->symbols;
  char label[33];
  int size = (m->size - m->symbol_start) / 36;
  int i;
  for(i = 0; i < size; i++){
    strncpy(symbol->label, m->data +m->symbol_start +i, 32);
    symbol->value = m->data[m->symbol_start +i +1];
    if(i < size -1){
      symbol->next = (llist*)malloc(sizeof(llist));
      symbol = symbol->next;
    }
  }
}

module *readModule(FILE *fp){
  module *mod = (module*)malloc(sizeof(module));
  int i;

  //determine size
  fseek(fp, 0, SEEK_END);
  mod->size = ftell(fp);

  mod->data = (int8_t*)malloc(mod->size);

  fseek(fp, 0, SEEK_SET);

  //read code and data starts
  fread(&mod->data_start, 4, 1, fp);
  fread(&mod->symbol_start, 4, 1, fp);

  fseek(fp, 0, SEEK_SET);
  
  //copy file to module
  for(i = 0; i < mod->size; i++)
    fread(mod->data +i, 1, 1, fp);
  
  return mod;

}

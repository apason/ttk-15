#include "linker.h"

#define CODESIZE 5

static void readSymbols(module *mod);
static void readCode(module *mod);
static module *readModule(FILE *fp);

//create all modules determined by argv
void createModules(int n, char **argv, module **modules){
  FILE *fp;
  int i;
  
  for(i = 0; i < n; i++){
    
    if((fp = fopen(argv[i], "rb")) == NULL){
      printf("ERROR: cannot open file: %s\n", argv[i]);
      exit(-1);
    }

    modules[i] = readModule(fp);

    fclose(fp);

    readCode(modules[i]);
    readSymbols(modules[i]);
    
  }
}

//initializes one module
static module *readModule(FILE *fp){
  module *mod = (module*)malloc(sizeof(module));
  int i;

  //determine size
  fseek(fp, 0, SEEK_END);
  mod->size = ftell(fp);

  mod->data = (int8_t*)malloc(mod->size);

  fseek(fp, 0, SEEK_SET);

  //read code and data starts
  fread(&mod->symbol_start, 4, 1, fp);
  fread(&mod->data_start, 4, 1, fp);

  fseek(fp, 0, SEEK_SET);
  
  //copy file to module
  for(i = 0; i < mod->size; i++)
    fread(mod->data +i, 1, 1, fp);
  
  return mod;

}

//constructs code array in module 
static void readCode(module *mod){
  int i;
  int size = (mod->data_start -8 ); //code starts from byte 8 by definition
  
  mod->codes = (char**)malloc(sizeof(char*) * size);
  
  for(i = 0; i < size / CODESIZE; i++){
    mod->codes[i] = (char*)malloc(sizeof(char) * CODESIZE);
    memcpy(mod->codes[i], mod->data +(i * CODESIZE) +8, CODESIZE);
  }
}

//constructs symbol list in module
static void readSymbols(module *mod){
  llist *symbol;
  int size = (mod->size - mod->symbol_start) / 36;
  int i;
  
  mod->symbols = (llist*)malloc(sizeof(llist));
  symbol = mod->symbols;

  for(i = 0; i < size; i++){
    strncpy(symbol->label, (char *)mod->data +mod->symbol_start +(i * 36) , 32);
    symbol->value = mod->data[mod->symbol_start +(i * 36) +32]; //type?
    if(i < size -1){
      symbol->next = (llist*)malloc(sizeof(llist));
      symbol = symbol->next;
      symbol->next = NULL;
    }
  }
}

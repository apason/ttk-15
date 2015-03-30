#include <linker.h>

#define SYMBOLSIZE 34  //size of one symbol sub block in .o15 format

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
  module *mod = (module *)malloc(sizeof(module));
  int i, data_size, code_size;

  //determine size
  fseek(fp, 0, SEEK_END);
  mod->size = ftell(fp);

  mod->data = (char *)malloc(mod->size);
  fseek(fp, 0, SEEK_SET);

  //read code and data starts
  fread(&mod->symbol_start, 4, 1, fp);
  fread(&mod->data_start, 4, 1, fp);

  //calculate linked size 
  code_size = 4 * (mod->data_start - CODESTART) / 5;
  data_size = mod->symbol_start - mod->data_start;
  mod->linked_size = code_size + data_size;

  fseek(fp, 0, SEEK_SET);
  
  //copy file to module
  for(i = 0; i < mod->size; i++)
    fread(mod->data +i, 1, 1, fp);
  
  return mod;
}

//constructs code array in module 
static void readCode(module *mod){
  int i;
  int size = (mod->data_start -CODESTART );
  
  mod->codes = (char **)malloc(sizeof(char *) * size);
  
  for(i = 0; i < size / CODESIZE; i++){
    mod->codes[i] = (char *)malloc(sizeof(char) * CODESIZE);
    memcpy(mod->codes[i], mod->data +(i * CODESIZE) +CODESTART, CODESIZE);
  }
}

//constructs symbol list in module
static void readSymbols(module *mod){
  llist *symbol;
  int size = (mod->size - mod->symbol_start) / SYMBOLSIZE;
  int i;
  
  mod->symbols = (llist *)malloc(sizeof(llist));
  symbol = mod->symbols;

  for(i = 0; i < size; i++){
    
    strncpy(symbol->label, (char *)mod->data +mod->symbol_start +(i * SYMBOLSIZE), sizeof(MYTYPE));
    symbol->value = *((int16_t *)(mod->data +mod->symbol_start +(i * SYMBOLSIZE) +LABELLENGTH));

    if(i < size -1){
      
      symbol->next = (llist *)malloc(sizeof(llist));
      symbol = symbol->next;
      symbol->next = NULL;
      
    }
    
  }
}





















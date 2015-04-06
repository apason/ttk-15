#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

//project header
#include <ttk-15.h>

#define BUFSIZE 32

int loadFile(MYTYPE *mem, char *file){
  FILE *fp = NULL;
  int   i  = 0;

  fp = fopen(file, "rb");
  
  if(fp == NULL){
    perror("Opening .b15 file");
    exit(-1);
  }

  for(i = 0; !feof(fp); i++)
    fread(mem +i, sizeof(MYTYPE), 1, fp);
    
    
  return i;
}

//loads old ttk-91 "binary" file to memory
int loadFile91(MYTYPE *mem, char *file){
  FILE     *fp     = NULL;
  uint32_t  cstart = -1;
  uint32_t  cend   = -1;
  uint32_t  dstart = -1;
  uint32_t  dend   = -1;
  int       i      =  0;
  
  char      buffer[BUFSIZE];

  fp = fopen(file, "r");
  
  if(fp == NULL){
    perror("fopen");
    exit(-1);
  }

  do{
    fgets(buffer, BUFSIZE, fp);
  }while(sscanf(buffer,"%d %d", &cstart, &cend) != 2);

  for(i = cstart; i <= cend; i++){
    fgets(buffer, BUFSIZE, fp);
    sscanf(buffer, "%d", mem +i);
  }
 
  do{
    fgets(buffer, BUFSIZE, fp);
  }while(sscanf(buffer, "%d %d", &dstart, &dend) != 2);

  for(i = dstart; i <= dend; i++){
    fgets(buffer, BUFSIZE, fp);
    sscanf(buffer, "%d", mem +i);
  }
  
  fclose(fp);

  return dend;
  
}

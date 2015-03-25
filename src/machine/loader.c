#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//project header
#include <ttk-91.h>

#define BUFSIZE 32

//loads old ttk-91 "binary" file to memory
int loadFile(MYTYPE *mem, char *file){
  FILE *fp;
  // c stands for "code" and d for "data"
  int cstart, cend, dstart, dend, i;
  char buffer[BUFSIZE];

  if( (fp = fopen(file,"r")) == NULL){
    printf("ERROR: cannot open binary file %s\n", file);
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

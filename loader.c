#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "ttk-91.h"

#define BUFSIZE 32

int loadFile(MYTYPE *mem, char *file){
  FILE *fp;
  int cstart,cend,dstart,dend,i;
  char buffer[BUFSIZE];

  if((fp=fopen(file,"r"))==NULL){
    printf("ERROR: cannot open binary file %s\n", file);
    exit(-1);
  }
  
  do{
    fgets(buffer,BUFSIZE,fp);
    printf("%s",buffer);
  }while(sscanf(buffer,"%d %d",&cstart,&cend)!=2);
  printf("cstart: %d\ncend: %d\n",cstart,cend);


  for(i=cstart;i<=cend;i++){
    fgets(buffer,BUFSIZE,fp);
    sscanf(buffer,"%d",mem+i);
  }
  
  do{
    fgets(buffer,BUFSIZE,fp);
    printf("%s",buffer);
  }while(sscanf(buffer,"%d %d",&dstart,&dend)!=2);
  printf("dstart: %d\ndend: %d\n",dstart,dend);

  for(i=dstart;i<=dend;i++){
    fgets(buffer,BUFSIZE,fp);
    sscanf(buffer,"%d",mem+i);
  }
  
  fclose(fp);

  return dend;
  
}

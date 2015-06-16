#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <ttk-15.h>

int writeFile(MYTYPE *mem, int dend);

int main(int argc, char *argv[]){
  int     dend = -1;
  FILE   *fp   = NULL;
  MYTYPE *mem  = (MYTYPE *) malloc(sizeof(MYTYPE) * 512);

  if(argc != 2){
    fprintf(stderr, "Please give .b91 file as parameter!\n");
    exit(-1);
  }

  fp = fopen(argv[1], "rb");
  if(fp == NULL){
    perror("opening b91 file");
    exit(-1);
  }

  //1024 should be enough for tests
  dend = loadFile91(mem, fp, 1024);

  return writeFile(mem,dend);
}

int writeFile(MYTYPE *mem, int dend){
  int i;
  FILE *out;

  out = fopen("result.b15", "wb");
  if(!out){
    perror("writing result file");
    exit(-1);
  }

  for(i = 0; i <= dend; i++)
    if(fwrite(mem + i, sizeof(MYTYPE), 1, out) != 1)
      return -1;

  return 0;
}

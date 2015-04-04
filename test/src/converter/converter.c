#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <ttk-15.h>

int writeFile(MYTYPE *mem, int dend);

int main(int argc, char *argv[]){
  int dend;
  MYTYPE *mem = (MYTYPE *) malloc(sizeof(MYTYPE) * 256);

  if(argc != 2){
    fprintf(stderr, "Please give .b91 file as parameter!\n");
    exit(-1);
  }

  dend = loadFile91(mem, argv[1]);

  return writeFile(mem,dend);
}

int writeFile(MYTYPE *mem, int dend){
  int i;
  FILE *out;

  out = fopen("result.b15", "wb");
  if(!out){
    perror("fopen");
    exit(-1);
  }

  for(i = 0; i <= dend; i++)
    if(fwrite(mem + i, sizeof(MYTYPE), 1, out) != 1)
      return -1;

  return 0;
}

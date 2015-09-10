#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

//project header
#include <ttk-15.h>
#include <instructions.h>
#include "machine.h"

#define BUFSIZE 32

static int needFix(MYTYPE buf);

int loadFile(MYTYPE *mem, FILE *file, MYTYPE limit){
    int   i  = 0;

    for(i = 0; !feof(file); i++){
	//check for sufficient memory size
	if(i >= limit) return i;
	fread(mem +i, sizeof(MYTYPE), 1, file);
    }

    return i;
}

//loads old ttk-91 "binary" file to memory
int loadFile91(MYTYPE *mem, FILE *file, MYTYPE memsize){
    uint32_t  cstart = -1;
    uint32_t  cend   = -1;
    uint32_t  dstart = -1;
    uint32_t  dend   = -1;
    int       i      =  0;
    MYTYPE    buf    =  0;
  
    char      buffer[BUFSIZE];

    do{
	fgets(buffer, BUFSIZE, file);
    }while(sscanf(buffer,"%d %d", &cstart, &cend) != 2);

    for(i = cstart; i <= cend; i++){
	fgets(buffer, BUFSIZE, file);
	sscanf(buffer, "%d", &buf);
	if(needFix(buf))
	    buf &= 0xFFE7FFFF;

	if(i >= memsize) return -1;

	*(mem +i) = buf;
    }
 
    do{
	fgets(buffer, BUFSIZE, file);
    }while(sscanf(buffer, "%d %d", &dstart, &dend) != 2);

    for(i = dstart; i <= dend; i++){
	fgets(buffer, BUFSIZE, file);
	if(i >= memsize) return -1;

	sscanf(buffer, "%d", mem +i);
    }
  
    fclose(file);

    return dend;
  
}

//use mtl here!
static int needFix(MYTYPE buf){
    int opcode = buf >> 24;

    opcode &= 0x000000FF;

    if(opcode >= JUMP && opcode <= EXIT)
	return 1;

    return 0;
}

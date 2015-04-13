#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <ttk-15.h>
#include "compiler.h"

static void openFile(options *opts, char *filename);
static options *newOptions(void);

options *getOptions(int argc, char *argv[]){
  int      optch       = 0;
  char     optstring[] = "m:o:";
  options *opts        = newOptions();

  //suppose all arguments are source files
  opts->count = argc -1;
   
  while((optch = getopt(argc, argv, optstring)) != -1){
    if(optch == 'o'){
      opts->count -= 2;
      openFile(opts, optarg);
    }
    if(optch == 'm'){
      opts->count -= 2;
      if(!strncmp("k91", optarg, strlen(optarg)))
	opts->mode = TTK91;
      else if(!strncmp("k15", optarg, strlen(optarg)))
	opts->mode = TTK15;
      else{
	fprintf(stderr, "ERROR: unknown mode: %s\n", optarg);
	exit(-1);
      }
    }
  }

  return opts;
}

static options *newOptions(void){
  options *opts = (options *) malloc(sizeof(options));

  opts->output = NULL;
  opts->mode   = UNDEFINED;
  opts->count  = 0;

  return opts;
}

static void openFile(options *opts, char *filename){
  FILE *fp = fopen(filename, "rb");

  if(fp == NULL){
    perror("opening output file");
    exit(-1);
  }

  opts->output = fp;
}

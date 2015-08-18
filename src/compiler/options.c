#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <ttk-15.h>
#include "compiler.h"

static void openFile(FILE **outputs, char *filename);
static options *newOptions(void);

options *getOptions(int argc, char *argv[]){
  int      i           = 0;
  int      optch       = 0;
  char     optstring[] = "m:o:g";
  options *opts        = newOptions();

  //suppose all arguments are source files
  opts->count = argc -1;

  while((optch = getopt(argc, argv, optstring)) != -1){

    //special case! when -o is defined there is only one source file
    if(optch == 'o'){
      opts->count = 1;
      opts->outputs = (FILE **) malloc(sizeof(FILE *));
      openFile(opts->outputs, optarg);
    }

    //mode speficied. valid modes: k91, k15. otherwise error caused.
    if(optch == 'm'){
      opts->count -= 2;
      
      if(!strncmp("k91", optarg, strlen(optarg)))      opts->mode = TTK91;
      else if(!strncmp("k15", optarg, strlen(optarg))) opts->mode = TTK15;
      else{
	fprintf(stderr, "ERROR: unknown mode: %s\n", optarg);
	exit(-1);
      }
    }

    //check if there is -g flag for debugging.
    if(optch == 'g'){
      opts->debug = ON;
      opts->count -= 1;
    }
  }

  //flag -o not used. there may be numerous of source files
  if(opts->outputs == NULL){
    opts->outputs = (FILE **) malloc(opts->count * sizeof(FILE *));

    for(i = 0; i < opts->count; i++)
      openFile(opts->outputs, argv[optind + i]);
  }

  return opts;
}

static options *newOptions(void){
  options *opts = (options *) malloc(sizeof(options));

  opts->outputs = NULL;
  opts->mode    = UNDEFINED;
  opts->count   = 0;
  opts->debug   = OFF;

  return opts;
}

static void openFile(FILE **outputs, char *filename){
  char  fn[MAX] = {'\0'};
  char *dot     = NULL;
  FILE *fp      = NULL;
  static int i  = 0;

  strncpy(fn, filename, MAX);
  dot = strrchr(fn, '.');

  if(dot == NULL)
    strncat(fn, ".o15", MAX -strlen(fn));
  else{
    *dot = '\0';
    strncat(fn, ".o15", MAX -strlen(fn)); //suffix
  }

  fp = fopen(fn, "wb");
	  
  if(fp == NULL){
    perror("opening output file");
    exit(-1);
  }
	  
  outputs[i++] = fp;
}

void freeOptions(options *opts) {
    free(opts->outputs);
    free(opts);
}

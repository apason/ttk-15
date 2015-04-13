#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include "linker.h"

static options *newOptions(void);
static void openFile(options *opts, char *filename);

options *getOptions(int argc, char *argv[]){
  int          optch       = 0;
  int          noflags     = 1;         //bubble gum!
  static char  optstring[] = "o:";
  options     *opts        = newOptions();

  while((optch = getopt(argc, argv, optstring)) != -1)
    if(optch == 'o'){
      openFile(opts, optarg);
      opts->count = argc -3;
      noflags = 0;
    }

  //use default, if filename is not given
  if(opts->output == NULL){
    openFile(opts, "a.out.b15");
    opts->count = argc -1;
  }

  //modules are now at the end of argv. lets change them!
  if(!noflags){
    argv[1] = argv[argc -1];
    argv[2] = argv[argc -2];
  }

  return opts;
}

static options *newOptions(void){
  options *opts = (options *) malloc(sizeof(options));

  opts->output = NULL;
  opts->count  = 0;

  return opts;
}

static void openFile(options *opts, char *filename){
  FILE *fp = fopen(filename, "wb");

  if(fp == NULL){
    perror("opening output file");
    exit(-1);
  }

  opts->output = fp;
}


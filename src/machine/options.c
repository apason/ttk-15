#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

//project headers
#include <ttk-15.h>
#include "machine.h"

#define DEFAULT_MEMSIZE 512

static options *newOptions(void);
static int      openFile(char *filename);
static int      getMode(char *filename);
static int      handleModeOption(int argc, char **argv, char *optstring);
static int      handleOtherOptions(int argc, char **argv, char *optstring);

static options *opts;

options *getOptions(int argc, char *argv[]){
  static char optstring[] = "f:m:gM:";

  opts = newOptions();

  //no flags used
  if(argc == 2){
    opts->mode = getMode(argv[1]);
    if(openFile(argv[1]) != 0){
      freeOptions(opts);
      return NULL;
    }
    return opts;
  }

  //handle mode
  if(handleModeOption(argc, argv, optstring) == -1){
    freeOptions(opts);
    return NULL;
  }

  //handle other options
  if(handleOtherOptions(argc, argv, optstring) == -1){
    freeOptions(opts);
    return NULL;
  }

  //open file if its not already open
  if(opts->file == NULL){
    if(opts->mode == UNDEFINED)
      opts->mode = getMode(argv[argc -1]);
    if(openFile(argv[argc -1]) != 0){
      freeOptions(opts);
      return NULL;
    }
  }
    
  return opts;
}

static int handleModeOption(int argc, char **argv, char *optstring){
  int optch = 0;

  //seek from first parametern
  optind = 1;
  
  while((optch = getopt(argc, argv, optstring)) != -1){
    if(optch == 'm'){
      if(!strncmp(optarg, "b91", strlen(optarg))){
	opts->mode = TTK91;
	break;
      }
      else if(!strncmp(optarg, "b15", strlen(optarg))){
	opts->mode = TTK15;
	break;
      }
      else{
	fprintf(stderr, "ERROR: unknown mode %s\n", optarg);
	return -1;
      }
    }
  }
  return 0;
}

static int handleOtherOptions(int argc, char **argv, char *optstring){
  int memsize = 0;
  int optch   = 0;
  
  //reset optind
  optind = 1;

  while((optch = getopt(argc, argv, optstring)) != -1){

    //file flag
    if(optch == 'f'){
      if(opts->mode == UNDEFINED)  opts->mode = getMode(optarg);
      if(openFile(optarg) != 0)    return -1;
    }

    //debug flag
    if(optch == 'g')
      opts->debug = ON;

    //memory size
    if(optch == 'M'){
      if(sscanf(optarg, "%d", &memsize) != 1){
	fprintf(stderr, "ERROR: memory size must be integer\n");
	return -1;
      }
      opts->memsize = memsize;
    }
    
  }
  return 0;
}

static int openFile(char *filename){
  if(opts->mode == UNDEFINED){
    fprintf(stderr, "binary type not defined! Using TTK-15 as default!\n");
    opts->mode = TTK15;
  }

  if(opts->mode == TTK15){
    opts->file = fopen(filename, "rb");
    if(opts->file == NULL){
      perror("Opening binary file");
      return -1;
    }
  }
  
  if(opts->mode == TTK91){
    opts->file = fopen(filename, "r");
    if(opts->file == NULL){
      perror("Opening binary file");
      return -1;
    }
  }

  return 0;
}

//get mode from filename
static int getMode(char *filename){
  char *dot = strrchr(filename, '.');

  if(dot == NULL)
    return UNDEFINED;
  
  if(!strncmp(dot, ".b15", 5)) return TTK15;
  if(!strncmp(dot, ".b91", 5)) return TTK91;

  return UNDEFINED;
}

//create options and initialize to defaults
static options *newOptions(void){
  options *opts = (options*) malloc(sizeof(options));

  opts->mode    = UNDEFINED;
  opts->file    = NULL;
  opts->debug   = OFF;
  opts->memsize = DEFAULT_MEMSIZE;

  return opts;
}

void freeOptions(options *opts){
  fclose(opts->file);
  free(opts);
}

  

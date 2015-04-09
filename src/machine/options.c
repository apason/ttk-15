#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <ttk-15.h>

static options *newOptions(void);
static int      openFile(options *opts);
static int      getMode(char *filename);

options *getOptions(int argc, char *argv[]){
  int         optch       = 0;
  static char optstring[] = "f:m:";
  options    *opts        = newOptions();

  //we first need to determine the type of file
  while((optch = getopt(argc, argv, optstring)) != -1){
    if(optch == 'm'){
      if(!strncmp(optarg, "k91", strlen(optarg))){
	opts->mode = TTK91;
	break;
      }
      else if(!strncmp(optarg, "k15", strlen(optarg))){
	opts->mode = TTK15;
	break;
      }
      else{
	fprintf(stderr, "ERROR: unknown mode %s\n", optarg);
	return NULL;
      }
    }
  }

  //reset optind
  optind = 1;

  while((optch = getopt(argc, argv, optstring)) != -1){
    if(optch == 'f'){
      
      if(opts->mode == UNDEFINED) opts->mode = getMode(optarg);
      if(openFile(opts) != 0)
	return NULL;
    }
  }

  //filename is only argument
  if(opts->file == NULL && opts->mode == UNDEFINED){
    if(argc != 2){
      fprintf(stderr, "ERROR: invalid arguments!\n");
      return NULL;
    }
    if(opts->mode == UNDEFINED) opts->mode = getMode(argv[1]);
    if(openFile(opts) != 0)
      return NULL;
  }

  //mode defined, file is no
  if(openFile(opts) != 0)
    return NULL;
  
  return opts;
}

static int openFile(options *opts){
  if(opts->mode == UNDEFINED){
    fprintf(stderr, "binary type not defined! Using TTK-15 as default!\n");
    opts->mode = TTK15;
  }

  if(opts->mode == TTK15){
    opts->file = fopen(optarg, "rb");
    if(opts->file == NULL){
      perror("Opening binary file");
      return -1;
    }
  }
  else if(opts->mode == TTK91){
    opts->file = fopen(optarg, "r");
    if(opts->file == NULL){
      perror("Opening binary file");
      return -1;
    }
  }

  return 0;
}

static int getMode(char *filename){
  char *dot = strrchr(filename, '.');

  if(dot == NULL)
    return UNDEFINED;
  
  if(!strncmp(dot, ".k15", 5)) return TTK15;
  if(!strncmp(dot, ".k91", 5)) return TTK91;

  return UNDEFINED;
}


static options *newOptions(void){
  options *opts = (options*) malloc(sizeof(options));

  opts->mode = UNDEFINED;
  opts->file = NULL;

  return opts;
}

  

  

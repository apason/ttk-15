#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <ttk-15.h>

static options *newOptions(void);
static int      openFile(options *opts, char *filename);
static int      getMode(char *filename);
static int findM(int argc, char *argv[]);

options *getOptions(int argc, char *argv[]){
  int         optch       = 0;
  int         arg         = 0;
  static char optstring[] = "f:m:";
  options    *opts        = newOptions();

  //we first need to determine the type of file
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
	return NULL;
      }
    }
  }

  //reset optind
  optind = 1;

  while((optch = getopt(argc, argv, optstring)) != -1){
    if(optch == 'f'){
      
      if(opts->mode == UNDEFINED) opts->mode = getMode(optarg);
      if(openFile(opts, optarg) != 0)
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
    if(openFile(opts, argv[1]) != 0)
      return NULL;
  }

  //mode defined, file is no
  if(opts->mode != UNDEFINED && opts->file == NULL){
    if(argc != 4){
      fprintf(stderr, "ERROR: invalid arguments!\n");
      return NULL;
    }

    arg = findM(argc, argv);

    //set arg to filename argument
    if(arg == 1) arg = 3;
    else if(arg == 2) arg = 1;

    if(openFile(opts, argv[arg]) != 0)
      return NULL;
  
  }
  
  return opts;
}

//find parameter "-m" from argv
static int findM(int argc, char *argv[]){
  int i = 0;
  for(i = 0; i < argc; i++)
    if(!strncmp(argv[i], "-m", strlen(argv[i])))
       return i;

  //this can not happen!
  return -1;
}

static int openFile(options *opts, char *filename){
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
  else if(opts->mode == TTK91){
    opts->file = fopen(filename, "r");
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
  
  if(!strncmp(dot, ".b15", 5)) return TTK15;
  if(!strncmp(dot, ".b91", 5)) return TTK91;

  return UNDEFINED;
}


static options *newOptions(void){
  options *opts = (options*) malloc(sizeof(options));

  opts->mode = UNDEFINED;
  opts->file = NULL;

  return opts;
}

  

  

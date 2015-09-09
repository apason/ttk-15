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
    char     optstring[] = "m:o:cg";
    options *opts        = newOptions();

    //suppose all arguments are source files
    opts->count = argc -1;

    while((optch = getopt(argc, argv, optstring)) != -1){
        switch (optch) {
            //special case! when -o is defined there is only one source file
            case 'o':
                {
                    opts->count -= 2;
                    opts->boutput = (char*)malloc(sizeof(char) * strlen(optarg) + 1);
                    strcpy(opts->boutput, optarg);
                    break;
                }
            //just compile don't link the object modules
            case 'c':
                {
                    opts->count--;
                    opts->nolink = 1;
                    break;
                }

                //mode speficied. valid modes: k91, k15. otherwise error caused.
            case 'm':
                {
                    opts->count -= 2;

                    if(!strncmp("k91", optarg, strlen(optarg)))      opts->mode = TTK91;
                    else if(!strncmp("k15", optarg, strlen(optarg))) opts->mode = TTK15;
                    else{
                        fprintf(stderr, "ERROR: unknown mode: %s\n", optarg);
                        exit(-1);
                    }
                    break;
                }

                //check if there is -g flag for debugging.
            case 'g':
                {
                    opts->debug = ON;
                    opts->count -= 1;
                    break;
                }
        }
    }

    //flag -o not used. there may be numerous of source files
    if(opts->outputs == NULL){
        opts->outputs = (FILE **) malloc(opts->count * sizeof(FILE *));
        opts->filenames = (char **) malloc(opts->count * sizeof(char *));

        for(i = 0; i < opts->count; i++){
            openFile(opts->outputs, argv[optind + i]);
            opts->filenames[i] = (char*) malloc((strlen(argv[optind +i]) +1) * sizeof(char));
            strncpy(opts->filenames[i], argv[optind +i], strlen(argv[optind +i]) +1);

        }
    }

    return opts;
}

static options *newOptions(void){
    options *opts = (options *) malloc(sizeof(options));

    opts->outputs   = NULL;
    opts->mode      = UNDEFINED;
    opts->count     = 0;
    opts->debug     = OFF;
    opts->nolink    = 0;
    opts->filenames = NULL;
    opts->boutput   = NULL;

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
    int i;
    for (i = 0; i < opts->count; i++)
        free(opts->filenames[i]);
    free(opts->filenames);
    free(opts->boutput);
    free(opts);
}

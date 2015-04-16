#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "compiler.h"

int main (int argc, char* argv[]) {

    options *opts = getOptions(argc,argv);
    int debug = 0;
    //options now includes information of mode int opts->mode
    //outputfile in opts->output and number of files to compile
    //opts->count. source files are in end of argv as 
    //note. output file is already open!
    //SO. source files are argv[argc -opts->count] to argv[argc -1]

    if (argc < 2) return 0;
    int n;
    FILE** output_list = opts->outputs;
    for ( n = argc - opts->count; n < argc; ++n ) {


        FILE* output = *output_list++;
        // we take the first argument to be name of a .ttk91 file
        code_file codeFile;
        codeFile.name = argv[n];
        codeFile.mode = TTK91;
        codeFile.fh_out = output++;

        // read the codeFile
        if (readCodeFile(&codeFile, debug) < 0) return -1;

        // find the suffix of the file
        char* suffix = strrchr(argv[n], '.');
        if (opts->mode == UNDEFINED) {
            if (!strncmp(suffix,".k15",5))
                codeFile.mode = TTK15;
        } else
            codeFile.mode = opts->mode;


        // calculate code size and create the symbol table
        countSize(&codeFile);

        if (debug) {
            // print module size
            printf("module size = %d\n",codeFile.moduleSize);
        }

        // write the code file
        writeCodeFile(&codeFile);

        if (debug) {
            // print the symbol table
            printf("Symbols:\n");
            label_list* temp = codeFile.symbolList;
            while (temp != NULL) {
                printf("%s : %0x : %d : %d\n",temp->label, (uint16_t)temp->address, temp->size, temp->value);
                temp = temp->next;
            }
        }
        // free the space reserved for dynamic data in codeFile
        freeCodeFile(&codeFile);
    }

    return 0;
}

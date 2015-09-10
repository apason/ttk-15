#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "compiler.h"

int main (int argc, char* argv[]) {

    options *opts = getOptions(argc,argv);
    int debug = (opts->debug == ON);
    //options now includes information of mode int opts->mode
    //outputfile in opts->output and number of files to compile
    //opts->count. source files are in end of argv as 
    //note. output file is already open!
    //SO. source files are argv[argc -opts->count] to argv[argc -1]

    if (argc < 2) return 0;
    int n;
    FILE** output_list = opts->outputs;
    char** output_name_list = opts->filenames; 
    for ( n = argc - opts->count; n < argc; ++n ) {


        FILE* output = *output_list++;
        // we need this to delete the output file if there is an error
        char *output_name = *output_name_list++;
        strcpy(strrchr(output_name,'.'),".o15");

        // assume at first that it's a ttk91 file instead of ttk15
        // and set the code_file struct to hold the filehandle and output name
        code_file codeFile;
        codeFile.name = argv[n];
        codeFile.mode = TTK91 | (debug << 2);
        codeFile.fh_out = output++;

        // read the file into the struct
        if (readCodeFile(&codeFile, debug) < 0) {
            unlink(output_name);
            continue;
        }
        char* suffix = strrchr(argv[n], '.');
        if (opts->mode == UNDEFINED && suffix != NULL) {
            if (!strncmp(suffix,".k15",5))
            codeFile.mode = TTK15 | (debug << 2);
        } else
            codeFile.mode = opts->mode | (debug << 2);


        // calculate code size and create the symbol table
        if (buildModule(&codeFile) < 0) {
            freeCodeFile(&codeFile);
            unlink(output_name);
            continue;
        }

        if (debug) {
            // print module size
            printf("module size = %d\n",codeFile.moduleSize);
        }

        // write the code file
        if (writeCodeFile(&codeFile) < 0) {
            unlink(output_name);
        }

        if (debug) {
            const char* mode[4] = {
                "NO_LABEL", "LOCAL",
                "IMPORT", "EXPORT" };
            // print the symbol table
            printf("Symbols:\n");
            label_list* temp = codeFile.symbolList;
            while (temp != NULL) {
                printf("%s : %0x : %d : %d : %s\n",temp->label, (uint16_t)temp->address, temp->size, temp->value, mode[temp->mode]);
                temp = temp->next;
            }
            printf("Label locations:\n");
            usage_list* utemp = codeFile.usageList;
            while (utemp != NULL) {
                printf("%0x : %s \n",utemp->value, utemp->label);
                utemp = utemp->next;
            }
        }
        // free the space reserved for dynamic data in codeFile
        freeCodeFile(&codeFile);
    }
    
    // invoke linker now
    if (!opts->nolink) {
        output_name_list = opts->filenames;
        int currentsize = 60;
        char *linkercall = (char*)malloc(sizeof(char) * currentsize);
        strcpy(linkercall, "linker\0");
        // this branch's linker does not use -g that's why this line is commented
        /*if (debug) {
          strncat(linkercall, " -g\0", 4);
          }*/
        // -o means specified output binary
        if (opts->boutput != NULL) {
            strcat(linkercall, " -o ");
            strcat(linkercall, opts->boutput);
        }
        // calculate how much space is left in the linker call char array
        int spaceleft = currentsize - strlen(linkercall);
        for ( n = argc - opts->count; n < argc; ++n ) {
            char *currentobject = *output_name_list++;
            // double the size of the linker call array if the space is not enough
            if (strlen(currentobject) + 1 > spaceleft) {
                currentsize <<= 1;
                char *temp = (char*)malloc(sizeof(char) * currentsize);
                strcpy(temp, linkercall);
                free(linkercall);
                linkercall = temp;
            }
            strcat(linkercall, " ");
            strcat(linkercall, currentobject);
        }
        system(linkercall);
        free(linkercall);

        // delete the object files
        output_name_list = opts->filenames;
        for ( n = argc - opts->count; n < argc; ++n ) unlink(*output_name_list++);
    }

    // free the options struct
    freeOptions(opts);

    return 0;
}

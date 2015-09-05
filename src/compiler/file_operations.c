#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <ttk-15.h>
#include <instructions.h>
#include <module.h>
#include "errorcodes.h"
#include "compiler.h"

static int countLines(FILE*);
static int nextLine(FILE*);
static char** readCode(FILE*, int, int**, int);
static int writeInstruction(char* instr,char* val,code_file* file, int line);
static void freeSymbols(code_file*);
static void freeCodeArray(code_file*);
static void print_error(int error, int line);

// this is a function to be used outside this file
int readCodeFile(code_file* file, int debug) {
    FILE* fh = fopen(file->name,"r");
    if (fh == NULL) {
        fprintf(stderr, "Error opening file: %s\n",file->name);
        return -1;
    }

    if (debug)
         printf("file is : %s\n",file->name);

    if ((file->lines = countLines(fh)) <= 0) {
        fclose(fh);
        fprintf(stderr, "Error linecount <= 0, is it an empty file?\n");
        return -1;
    }
    if (debug)
        printf("lines = %d\n",file->lines);

    rewind(fh);

    // read code lines to an array
    file->array = readCode(fh, file->lines,&(file->code_text), debug);

    fclose(fh);
    return 0;
}

static char** readCode(FILE* fh, int lines, int**ppcode_text, int debug) {
    int i;
    int text_lines = 0;
    int* code_text = (int*) malloc((lines + 1)*sizeof(int));
    *ppcode_text = code_text;
    // reserve space and read the code lines from the file
    char** input = (char**) malloc(lines*sizeof(char*));
    for (i = 0; i < lines; ++i) {
        if (feof(fh)) {
            fprintf(stderr, "Error reading the source file!\n");
            return NULL;
        }
        int ch = fgetc(fh);

        // Skip comment lines and whitespaces
        do {
            while (isspace(ch)) {
                if (ch == '\n') ++text_lines;
                ch = fgetc(fh);
            }
            if (ch == ';') {
                ++text_lines;
                while (fgetc(fh) != '\n');
                ch = fgetc(fh);
            }
        } while (ch == ';' || ch == '\n');

        // reserve space for the next line
        input[i] = (char*) malloc(MAX * sizeof(char));
        int count = 0;
        while (ch != EOF && ch != '\n') {
            if (ch == ';') {
                while (fgetc(fh) != '\n');
                break;
            }

            input[i][count++] = ch;
            if(count >= MAX - 1)
                break;
            if (ch == ',')
                while (isspace(ch = fgetc(fh)));
            else
                ch = fgetc(fh);
        }
        input[i][count] = 0;
        code_text[i] = ++text_lines;
        if (debug)
            printf("Line %d: %s\n",i+1,input[i]);

    }
    return input;
}

static int countLines (FILE* fh) {
    if (fh == NULL) return -1;
    int lines = 0;
    while (!feof(fh)) {
        lines += nextLine(fh);
    }
    return lines;
}

static int nextLine(FILE* fh) {
    int ch = fgetc(fh);
    // Strip whitespaces from beginning
    while (ch != EOF && isspace(ch)) ch = fgetc(fh);
    // If this line is a comment, let's skip it
    if (ch == ';') {
        while ((ch = fgetc(fh)) != EOF && ch != '\n');
        return 0;
    }
    // If this line is empty, skip it too
    if (ch == '\n') {
        return 0;
    }
    // If we have reached end of file, there is no line
    if (ch == EOF) return 0;
    // Valid line so seek through and add 1 to line count
    while (ch != EOF && ch != '\n') ch = fgetc(fh);
    return 1;
}


// this is also a function to be used outside of this file
int writeCodeFile(code_file* file) {
    int error = 0;
    FILE* fh = file->fh_out;
    int debug = file->mode >> 2;
    // the usage list starts out null
    file->usageList = NULL;
    // skip header, we'll write it later
    fseek(fh, sizeof(MYTYPE) * 4, SEEK_CUR );

    // start writing data
    int i, cInstructions = 0;
    char word[MAX], label[LABELLENGTH], val[MAX];
    for (i = 0; i < file->lines; ++i) {
        // make sure no previous data is bothering us
        val[0] = '\0';

        sscanf(file->array[i], "%s %s", word, val);
        if (isInstruction(word)) {
            error = writeInstruction(word,val,file, cInstructions);
            // print error if found
            if (error < 0) {
                print_error(error, file->code_text[cInstructions]);
                fprintf(stderr,"\t\"%s\"\n",file->array[i]);
                // if error is not a warning ( > -10 ) end
                if (error > -10) {
                    fclose(fh);
                    return error;
                }
            }
            ++cInstructions;
            continue;
        }

        val[0] = '\0';
        char trash[MAX];
        if (sscanf(file->array[i], "%s %s %s", label, word, val) < 2) {
            fprintf(stderr,"Error reading line: %d\n",file->code_text[cInstructions]);//error
            fprintf(stderr,"\t\"%s\"\n",file->array[i]);
        }
        if (!strncmp(label, "export", LABELLENGTH))
            sscanf(file->array[i], "%s %s %s %s", trash, label, word, val);

        if (!isInstruction(label) && isInstruction(word)) {
            error = writeInstruction(word,val,file, cInstructions);
            // print error if found
            if (error < 0) {
                print_error(error, file->code_text[cInstructions]);
                fprintf(stderr,"\t\"%s\"\n",file->array[i]);
                // if error is not a warning ( > -10 ) end
                if (error > -10) {
                    fclose(fh);
                    return error;
                }
            }
            ++cInstructions;
        }

    }
    label_list* symbols = file->symbolList;
    // write the data segment
    while (symbols!=NULL) {
        if (symbols->mode != IMPORT) {
            if (symbols->size == 1) {
                fwrite(&(symbols->value),sizeof(MYTYPE),1,fh);
            } else if (symbols->size > 1) {
                for (i = 0; i < symbols->size; ++i) {
                    MYTYPE nul = 0;
                    fwrite(&nul, 1, sizeof(MYTYPE),fh);
                }
            }
        }
        symbols = symbols->next;	
    }
    
    // write the import and export tables
    file->exportSize = 0;
    file->importSize = 0;
    symbols = file->symbolList;
    while (symbols != NULL) {
        if (symbols->mode == EXPORT)
            file->exportSize += 34;
        if (symbols->mode == IMPORT)
            file->importSize += 34;
        if (symbols->mode == EXPORT || symbols->mode == IMPORT) {
            if (strlen(symbols->label)>32)
                fprintf(stderr, "Warning: symbol name more than 32 chars, will be cut: %s\n",symbols->label);
            fwrite(symbols->label,sizeof(char),32,fh);
            fwrite(&(symbols->address),sizeof(symbols->address),1,fh);
        }
        symbols = symbols->next;
    }
    usage_list *ulabels = file->usageList;
    if (debug) {
        while (ulabels != NULL) {
            fwrite(&(ulabels->value),sizeof(ulabels->value),1,fh);
            fwrite(ulabels->label, sizeof(char), 32, fh);
            ulabels = ulabels->next;
        }
    }

    fseek(fh, 0, SEEK_SET);

    // write header to the object file
    // 4 * 4 is the header size in bytes
    MYTYPE dataSegmentAddress = file->codeSize*5 + 4 * 4;
    MYTYPE exportTableAddress = dataSegmentAddress + (file->moduleSize - file->codeSize)*4;
    MYTYPE importTableAddress = exportTableAddress + file->exportSize;
    MYTYPE labelUsageAddress = importTableAddress + file->importSize;

    fwrite(&dataSegmentAddress, sizeof(MYTYPE),1,fh);
    fwrite(&exportTableAddress, sizeof(MYTYPE),1,fh);
    fwrite(&importTableAddress, sizeof(MYTYPE),1,fh);
    fwrite(&labelUsageAddress, sizeof(MYTYPE),1,fh);
    fclose(fh);
    return 0;
}


static int writeInstruction(char* word,char* val,code_file* file, int line) {
    FILE* fh = file->fh_out;
    
    int warning = 0;

    uint8_t firstByte = NO_LABEL;
    uint32_t instruction = 0;
    int ttk_15 = (file->mode & TTK15);

    int opCode = 0;
    int reg = 0;
    int mode = 0;
    int ireg = 0;
    int addr = 0;

    int nargs = 0;

    // find out the operation code
    opCode = getOpCode(word);
    if (opCode == -1) return INVALIDOPCODE;

    // find number of arguments
    char* argument = NULL;

    if (val[0]) {
        // make the arguments lowercase
        {
            char* p = val;
            for (; *p; ++p) *p = tolower(*p);
        }

        // split val into two if there's a comma
        argument = strchr(val, ',');
        if (argument != NULL && strlen(argument) > 1) {
            *argument++ = '\0';
            nargs = 2;
        } else {
            nargs = 1;
            argument = val;
        }

    } else
        nargs = 0;

    // if we have arguments for the instruction
    if (nargs) {
        int temp;
        // get indexing mode
        if ((temp = getIndexingMode(argument)) < 0)
            return INVALIDMODE;
        if (temp != 1)
            ++argument;
        mode = temp;
        // get index register
        if ((ireg = getIndexRegister(argument)) < 0)
            return INVALIDIREG;
        // if we have two arguments, first one has to be a register
        if (nargs == 2) {
            // Get first argument which is a register
            if ((reg = getRegister(val, 1)) < 0)
                return INVALIDREG;
            // check if opcode is STORE and make it compatible if we use ttk-91
            if (( opCode == STORE || (opCode >= JUMP && opCode <= CALL)) && !ttk_15) {
                if (--mode < 0)
                    return INVALIDMODE;
            }
        }
        // If the second argument is also a register it's used as an index register
        // otherwise it actually is the first argument
        // If it's not a register it's supposed to be the address part of the command
        if (( temp = getRegister(argument, 0)) >= 0) {
            if (nargs == 2) {
                ireg = temp;
                if (--mode < 0)
                    return INVALIDMODE;
            } else
                reg = temp;
        } else {
            // this variable also stores possible error in getAddress
            int isItFloat = 0;
            addr = getAddress(argument, file, &firstByte, &isItFloat, line);
            if (isItFloat < 0)
                return isItFloat;
            // If the argument is a float we have to use fload instead of just load
            if (isItFloat) {
                if (opCode == LOAD)
                    opCode = FLOAD;
                // float arguments should not be found in other commands
                 else if (opCode < FLOAD) {
                     warning = FLOATARGUMENT;
                 }
            }
        }
    
    }


    instruction |= (opCode << 24);
    instruction |= (reg << 21);
    instruction |= (mode << 19);
    instruction |= (ireg << 16);
    // only use 16 bits for the address
    instruction |= (addr & 0xffff);

    // write 1, if address is a label, 0, if not
    fwrite(&firstByte,sizeof(firstByte),1,fh);
    // write the compiled instruction
    fwrite(&instruction,sizeof(instruction),1,fh);
    return warning;
}

void freeCodeFile(code_file* file) {
    freeSymbols(file);
    freeCodeArray(file);
}

static void freeSymbols(code_file* file) {
    while (file->symbolList != NULL) {
        label_list* temp = file->symbolList;
        file->symbolList = file->symbolList->next;
        free(temp);
    }
    while (file->usageList != NULL) {
        usage_list* next = file->usageList->next;
        free(file->usageList);
        file->usageList = next;
    }
}

static void freeCodeArray(code_file* file) {
    int i;
    for (i = 0; i < file->lines; ++i) {
        free(file->array[i]);
    }
    free(file->array);
    free(file->code_text);
}

static void print_error(int error, int line) {
    switch(error) {
        case FLOATARGUMENT:
            fprintf(stderr,"WARNING: Using float argument on line: %d\n",line);
            break;
        case INVALIDOPCODE:
            fprintf(stderr,"ERROR: Invalid operation code on line: %d\n",line);
            break;
        case INVALIDMODE:
            fprintf(stderr,"ERROR: Invalid mode on line: %d\n",line);
            break;
        case INVALIDIREG:
            fprintf(stderr,"ERROR: Invalid index register on line: %d\n",line);
            break;
        case INVALIDREG:
            fprintf(stderr,"ERROR: Invalid register on line: %d\n",line);
            break;
        case BIGFLOATEXP:
            fprintf(stderr,"ERROR: Too big exponent on line: %d\n",line);
            break;
        default:
            fprintf(stderr,"ERROR: Unknown syntax error on line: %d\n",line);
    }
}

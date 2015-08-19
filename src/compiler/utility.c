#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <module.h>
#include <ttk-15.h>
#include "compiler.h"
#include "errorcodes.h"


f16 f16Encode(float f, int *error){

    if(f == 0)	return 0;

    int32_t n = *(volatile int32_t*) &f;
    
    int32_t sign = (n & 0x80000000);
    int32_t exp  = (n & 0x7f800000) >> 23;
    int32_t man  = (n & 0x7fffff);

    if(exp -127 > 16 || exp -127 < -16) {
       printf("exponent too big!\n");
       *error = BIGFLOATEXP;
       
    }
    if(man & 0x1fff) {
        printf("mantissa too big!\n");
    }

    return (f16) ((((sign >> 16) | ( (exp -127 +15) << 10)) & ~0x3ff) | (man >> 13));
}

int isInstruction(char *word){
    char *p = word;
    // make word lowercase
    for (; *p; ++p) *p = tolower(*p);
    int i;
    static char instructions[N_INSTR][6] = {"nop", "store", "load", "in", "out",\
        "add", "sub", "mul", "div", "mod",\
            "and", "or", "xor", "shl", "shr", "not", "shra",\
            "comp", "jump", "jneg", "jzer", "jpos", "jnneg", "jnzer", "jnpos",\
            "jles", "jequ", "jgre", "jnles", "jnequ", "jngre",\
            "call", "exit", "push", "pop", "pushr", "popr", "svc",\
            "fload", "fin", "fout", "fadd", "fsub", "fmul", "fdiv", "fcomp",\
            "fjneg", "fjzer", "fjpos", "fjnneg", "fjnzer", "fjnpos"};
    for(i = 0; i < N_INSTR; i++)
        if(!strncmp(instructions[i], word, LABELLENGTH))
            return i + 1;
    return 0;

}

int getHardcodedSymbolValue(char* arg) {
    if (!strncmp(arg, "crt", 4))
        return 0x0;
    else if (!strncmp(arg, "kbd", 4))
        return 0x1;
    else if (!strncmp(arg, "stdin", 6))
        return 0x6;
    else if (!strncmp(arg, "stdout", 7))
        return 0x7;
    else if (!strncmp(arg, "halt", 5))
        return 0xB;
    else if (!strncmp(arg, "read", 5))
        return 0xC;
    else if (!strncmp(arg, "write", 6))
        return 0xD;
    else if (!strncmp(arg, "time", 5))
        return 0xE;
    else if (!strncmp(arg, "date", 5))
        return 0xF;
    return -1;
}

int getOpCode(char* word) {
    static const char opcodes[N_INSTR][8] = {\
        "nop\0\0\0\0\0",\
            "store\0\0\x1",\
            "load\0\0\0\x2",\
            "in\0\0\0\0\0\x3",\
            "out\0\0\0\0\x4",\
            "add\0\0\0\0\x11",\
            "sub\0\0\0\0\x12",\
            "mul\0\0\0\0\x13",\
            "div\0\0\0\0\x14",\
            "mod\0\0\0\0\x15",\
            "and\0\0\0\0\x16",\
            "or\0\0\0\0\0\x17",\
            "xor\0\0\0\0\x18",\
            "shl\0\0\0\0\x19",\
            "shr\0\0\0\0\x1A",\
            "not\0\0\0\0\x1B",\
            "shra\0\0\0\x1C",\
            "comp\0\0\0\x1F",\
            "jump\0\0\0\x20",\
            "jneg\0\0\0\x21",\
            "jzer\0\0\0\x22",\
            "jpos\0\0\0\x23",\
            "jnneg\0\0\x24",\
            "jnzer\0\0\x25",\
            "jnpos\0\0\x26",\
            "jles\0\0\0\x27",\
            "jequ\0\0\0\x28",\
            "jgre\0\0\0\x29",\
            "jnles\0\0\x2A",\
            "jnequ\0\0\x2B",\
            "jngre\0\0\x2C",\
            "call\0\0\0\x31",\
            "exit\0\0\0\x32",\
            "push\0\0\0\x33",\
            "pop\0\0\0\0\x34",\
            "pushr\0\0\x35",\
            "popr\0\0\0\x36",\
            "svc\0\0\0\0\x70",\
            "fload\0\0\x82",\
            "fout\0\0\0\x84",\
            "fadd\0\0\0\x91",\
            "fsub\0\0\0\x92",\
            "fmul\0\0\0\x93",\
            "fdiv\0\0\0\x94",\
            "fcomp\0\0\x9F",\
            "fjneg\0\0\xA1",\
            "fjzer\0\0\xA2",\
            "fjpos\0\0\xA3",\
            "fjnneg\0\xA4",\
            "fjnzer\0\xA5",\
            "fjnpos\0\xA6"
    };
    int i;
    for (i = 0; i < N_INSTR; ++i) {
        if (strncmp(opcodes[i],word,6) == 0) {
            return opcodes[i][7];
        }
    }
    fprintf(stderr,"Unknown opcode: %s\n",word);
    return -1;
}


int getIndexingMode(char* argument) {
    if (argument[0] == '=') {
        if (argument[1] == '\0') {
            fprintf(stderr, "Invalid argument: %s\n", argument);
            return -1;
        }
        return 0;
    } else if (argument[0] == '@') {
        if (argument[1] == '\0') {
            fprintf(stderr, "Invalid argument: %s\n", argument);
            return -1;
        }
        return 2;
    }
    else
        return 1;
}

int getRegister(char* val, int errors) {

    int reg = 0;
    if (tolower(val[0]) == 'r' && isdigit(val[1]) && val[2] == 0) {
        reg = atoi(val + 1);
        if (reg > 7) {
            if (errors) fprintf(stderr, "Invalid register r%d\n",reg);
            return -1;
        }
    } else if (tolower(val[1]) == 'p' && val[2] == 0) {
        if (tolower(val[0] == 's'))
            reg = 6;
        else if (tolower(val[0] == 'f'))
            reg = 7;
        else {
            if (errors) fprintf(stderr, "First operand must be a register, found: %s\n",val);
            return -1;
        }
    } else {
        if (errors) fprintf(stderr, "First operand must be a register, found: %s\n",val);
        return -1;
    }
    return reg;
}

int getIndexRegister(char* argument) {

    // figure if there is a need for indexing register
    char* bracket = argument;
    while (*bracket && (*bracket != '(')) ++bracket;
    if (*bracket == '(') {
        // found braces, inside must be a register
        *bracket++ = '\0';
        char* p = bracket;
        while (*p && *p != ')') ++p;
        if (*p == ')') {
            *p = '\0';
            return getRegister(bracket,1);
        } else {
            fprintf(stderr, "Missing closed braces!\n");
            return -1;
        }
    }
    return 0;
}

int getAddress(char* argument, label_list* symbols, uint8_t *firstByte, int *isItFloat) {
    label_list* temp = symbols;
    int addr;
    // is the address a number?
    if (isdigit(argument[0]) || (isdigit(argument[1]) && argument[0] == '-')) {

        char * s = argument;
        if (*s == '-') ++s;
        while (*s) if (!isdigit(*s++)) break;
        if (!*s)
            addr = atoi(argument);
        else {
            int error = 0;
            f16 converted = f16Encode(atof(argument), &error);
            if (error < 0) {
                *isItFloat = error;
                return 0;
            }
            uint16_t *temporary = &converted;
            addr = (int) *temporary;
            *isItFloat = 1;
        }
            
        // is it not a hardcoded symbol, but a label?
    } else if ((addr = getHardcodedSymbolValue(argument)) < 0) {
        int16_t index = -1;
        while(temp != NULL) {
            // when label is found it's address is replaced in the instruction
            if (!strncmp(temp->label,argument,strlen(argument))) {
                addr = temp->address;
                // make sure equ are not treated as labels
                if (temp->size < 0) *firstByte = NO_LABEL;
                else    *firstByte = temp->mode;
                break;
            }
            // don't use the same index as the other external labels
            if (temp->address < 0) --index;
            temp = temp->next;
        }
        if (temp == NULL) {
            // Didn't find label so added to the list of external symbols
            temp = symbols;
            while (temp->next != NULL) temp = temp->next;
            temp->next = (label_list*)malloc(sizeof(label_list));
            temp = temp->next;
            temp->next = NULL;
            // zero the label char array
            memset(temp->label,0,sizeof(temp->label));
            strncpy(temp->label,argument,LABELLENGTH);
            temp->size = 0;
            temp->address = (int16_t)index;
            temp->mode = *firstByte = IMPORT;
            addr = index;
        }
    }
    return addr;
}

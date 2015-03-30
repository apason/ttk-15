#ifndef COMPILER
#define COMPILER
#include <linker.h>
#include <stdint.h>
#include <ttk-15.h>
#define MAX 128
// single linked list for the symbol table
typedef struct label_list {
	struct label_list* next;
	char label[LABELLENGTH + 1];
	int16_t address;
	MYTYPE size;
	// used only with the dc keyword
	MYTYPE value;
} label_list;
// a structure for data from the file
typedef struct {
	char* name;
	int lines;
	char** array;
	struct label_list* symbolList;
	MYTYPE moduleSize;
	MYTYPE codeSize;
	char out_name[MAX];
} code_file;
// reads the code file into the array in the struct and puts lenght in lines member
int readCodeFile(code_file*);
// free space for code_file
void freeCodeFile(code_file*);
// counts the size of the code and creates the symbol table
int countSize(code_file*);
// writes the binary file
int writeCodeFile(code_file*);
// check's if word is a valid instruction
int isInstruction(char *word);
#endif 

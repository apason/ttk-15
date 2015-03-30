#ifndef COMPILER
#define COMPILER
#define MAX 128
#include <stdint.h>
#include <ttk-91.h>
// single linked list for the symbol table
typedef struct label_list {
	struct label_list* next;
	char label[MAX];
	int16_t address;
	MYTYPE size;
	// used only with the dc keyword
	int value;
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
// counts the size of the code and creates the symbol table
int countSize(code_file*);
// writes the binary file
int writeCodeFile(code_file*);

int isInstruction(char *word);
#endif 

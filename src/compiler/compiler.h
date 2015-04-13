#ifndef COMPILER
#define COMPILER

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
	int mode;
	char** array;
	struct label_list* symbolList;
	MYTYPE moduleSize;
	MYTYPE codeSize;
	FILE* fh_out;
	int* code_text;
} code_file;

typedef struct OPTIONS {
  int    mode;
  int    count;
  int    debug;
  FILE **outputs;

} options;
// reads the code file into the array in the struct and puts lenght in lines member
int readCodeFile(code_file*, int);
// free space for code_file
void freeCodeFile(code_file*);
// counts the size of the code and creates the symbol table
int countSize(code_file*);
// writes the binary file
int writeCodeFile(code_file*);
// check's if word is a valid instruction
int isInstruction(char *word);
//options
options *getOptions(int argc, char *argv[]);
//from utility.h
extern int getHardcodedSymbolValue(char* argument);
extern int getOpCode(char* operation);
extern int getRegister(char* argument, int errors);
extern int getIndexingMode(char* argument);
extern int getIndexRegister(char* argument);
extern int getAddress(char* argument, label_list* symbols, uint8_t* firstByte);
#endif 
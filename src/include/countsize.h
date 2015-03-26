#ifndef COMPILER
#define COMPILER

typedef struct {
	char* name;
	int lines;
	char** array;
} code_file;

typedef struct label_list {
	struct label_list* next;
	char label[128];
	int address;
	int size;
} label_list;

int countSize(char**, int, label_list*);
int countLines(FILE*);
char** readCode(FILE*,int);

#endif 

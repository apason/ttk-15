#ifndef COMPILER_FILE_OPERATIONS
#define COMPILER_FILE_OPERATIONS
typedef struct {
	char* name;
	int lines;
	char** array;
} code_file;
int readCodeFile(code_file*);
#endif 

typedef struct {
	char* name;
	int lines;
	char** array;
} code_file;
int readCodeFile(code_file*);

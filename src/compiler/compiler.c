#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <compiler.h>

int main (int argc, char* argv[]) {
	if (argc < 2) return -1;
	if (strlen(argv[1]) >= MAX) {
		fprintf(stderr, "Too long filename: %s\n",argv[1]);
		return -1;
	}

	// we take the first argument to be name of a .ttk91 file
	code_file codeFile;
	codeFile.name = argv[1];

	// read the codeFile
	if (readCodeFile(&codeFile) < 0) return -1;

	// find the suffix of the file
	char* suffix = strrchr(argv[1], '.');
	if (suffix && strlen(suffix) > 3) {
		strncpy(suffix, ".o15",4);
	}
	if (argc == 4 && !strncmp(argv[2],"-o",3)) {
		strncpy(codeFile.out_name,argv[3],MAX - 1);
	} else {
		strncpy(codeFile.out_name,argv[1],MAX);
	}
	codeFile.out_name[MAX-1] = '\0';

	// calculate code size and create the symbol table
	countSize(&codeFile);

	// print module size
	printf("module size = %d\n",codeFile.moduleSize);

	// write the code file
	writeCodeFile(&codeFile);

	// print the symbol table
	printf("Symbols:\n");
	label_list* temp = codeFile.symbolList;
	while (temp != NULL) {
		printf("%s : %0x : %d : %d\n",temp->label, (uint16_t)temp->address, temp->size, temp->value);
		temp = temp->next;
	}

	// free the space reserved for dynamic data in codeFile
	freeCodeFile(&codeFile);

	return 0;
}

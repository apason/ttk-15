#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <compiler.h>

int main (int argc, char* argv[]) {
	int i;
	if (argc < 2) return -1;

	// we take the first argument to be name of a .ttk91 file
	code_file codeFile;
	codeFile.name = argv[1];
	if (argc == 4 && !strncmp(argv[2],"-o",3)) {
		strncpy(codeFile.out_name,argv[3],MAX - 1);
	} else {
		strncpy(codeFile.out_name,argv[1],MAX - 5);
		strncat(codeFile.out_name,".o15",4);
	}
	codeFile.out_name[MAX-1] = '\0';

	// read the codeFile
	if (readCodeFile(&codeFile) < 0) return -1;

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
		printf("%s : %0x : %d : %d\n",temp->label, temp->address, temp->size, temp->value);
		temp = temp->next;
	}

	// free the space of the label_list
	while (codeFile.symbolList != NULL) {
		label_list* temp = codeFile.symbolList;
		codeFile.symbolList = codeFile.symbolList->next;
		free(temp);
	}

	// free the space of the file array
	for (i = 0; i < codeFile.lines; ++i) {
		free(codeFile.array[i]);
	}
	free(codeFile.array);
	return 0;
}

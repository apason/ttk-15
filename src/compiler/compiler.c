#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"

int main (int argc, char* argv[]) {
	int i;
	if (argc < 2) return -1;

	// we take the first argument to be name of a .ttk91 file
	code_file codeFile;
	codeFile.name = argv[1];
	// read the codeFile
	if (readCodeFile(&codeFile) < 0) return -1;

	// make a new label list
	label_list* first = (label_list*) malloc(sizeof(label_list));
	int moduleSize = countSize(codeFile.array,codeFile.lines,first);
	printf("module size = %d\n",moduleSize);

	// print the symbol table
	printf("Symbols:\n");
	label_list* temp = first;
	while (temp != NULL) {
		printf("%s : %0x : %d\n",temp->label, temp->address, temp->size);
		temp = temp->next;
	}

	// free the space of the label_list
	while (first != NULL) {
		label_list* temp = first;
		first = first->next;
		free(temp);
	}

	// free the space of the file array
	for (i = 0; i < codeFile.lines; ++i) {
		free(codeFile.array[i]);
	}
	free(codeFile.array);
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"

int countLines(FILE*);
char** readCode(FILE*,int);

int main (int argc, char* argv[]) {
	int i;
	if (argc < 2) return -1;

	// we take the first argument to be name of a .ttk91 file
	char* filename = argv[1];

	printf("file is : %s\n",filename);

	// open file
	FILE* fh = fopen(filename,"r");
	if (fh == NULL) {
		fprintf(stderr, "Error opening file: %s\n",filename);
		return -1;
	}
	// count lines, but not comments
	int lines = countLines(fh);
	printf("lines = %d\n",lines);

	if (lines == -1) return -1;
	rewind(fh);
	// read code lines to an array
	char** codeLines = readCode(fh, lines);
	
	fclose(fh);

	// make a new label list
	label_list* first = (label_list*) malloc(sizeof(label_list));
	int moduleSize = countSize(codeLines,lines,first);
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
	for (i = 0; i < lines; ++i) {
		free(codeLines[i]);
	}
	free(codeLines);
	return 0;
}

int nextLine(FILE* fh) {
	int ch = fgetc(fh);
	// Strip whitespaces and tabs from beginning
	while (ch != EOF && ch == ' ' || ch == '\t') ch = fgetc(fh);
	// If this line is a comment, let's skip it
	if (ch == ';') {
		while ((ch = fgetc(fh)) != EOF && ch != '\n');
		return 0;
	}
	// If this line is empty, skip it too
	if (ch == '\n') {
		fgetc(fh);
		return 0;
	}
	// If we have reached end of file, there is no line
	if (ch == EOF) return 0;
	// Valid line so seek through and add 1 to line count
	while (ch != EOF && ch != '\n') ch = fgetc(fh);
	return 1;
}

int countLines (FILE* fh) {
	if (fh == NULL) return -1;
	int lines = 0;
	int ch = 0;	
	while (!feof(fh)) {
		lines += nextLine(fh);
	}
	return lines;
}

char** readCode(FILE* fh, int lines) {
	int i;

	// reserve space and read the code lines from the file
	char** input = (char**) malloc(lines*sizeof(char*));
	for (i = 0; i < lines; ++i) {
		if (feof(fh)) {
			fprintf(stderr, "Error reading the source file!\n");
			return -1;
		}
		int ch = fgetc(fh);
		// skip whitespaces
		while (ch == ' ' || ch == '\t') ch = fgetc(fh);
		// Skip comment lines
		while (ch == ';' || ch == '\n') {
			if (ch == '\n') {
				ch = fgetc(fh);
				continue;
			}
			while (fgetc(fh) != '\n');
			ch = fgetc(fh);
			while (ch == ' ' || ch == '\t') ch = fgetc(fh);
		}
		// reserve space for the next line
		input[i] = (char*) malloc(256 * sizeof(char));
		int count = 0;
		while (ch != EOF && ch != '\n') {
			if (ch == ';') {
				while (fgetc(fh) != '\n');
				break;
			}
			input[i][count++] = ch;
			ch = fgetc(fh);
		}
		input[i][count] = 0;
		printf("Line %d: %s\n",i+1,input[i]);
	
	}
	return input;
}

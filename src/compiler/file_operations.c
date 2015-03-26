#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <compiler.h>

int countLines(FILE*);
int nextLine(FILE*);
char** readCode(FILE*, int);
uint32_t convertIntoBytes(char* word, char* val, label_list* symbols);

// this is a function to be used outside this file
int readCodeFile(code_file* file) {
	FILE* fh = fopen(file->name,"r");
	if (fh == NULL) {
		fprintf(stderr, "Error opening file: %s\n",file->name);
		return -1;
	}
	
	printf("file is : %s\n",file->name);
	
	if ((file->lines = countLines(fh)) <= 0) {
		fclose(fh);
		return -1;
	}
	printf("lines = %d\n",file->lines);
	
	rewind(fh);

	// read code lines to an array
	file->array = readCode(fh, file->lines);

	fclose(fh);
	return 0;
}

// this is also a function to be used outside of this file
int writeCodeFile(code_file* file) {
	label_list* symbols = file->symbolList;
	// reserve space for the module
	uint32_t* array = (uint32_t*)malloc(sizeof(uint32_t) * file->moduleSize);
	label_list* temp = symbols;
	// set memory spaces reserved with dc to their correct value
	while (temp->next != NULL) {
		if (temp->size == 1) {
			if (temp->address >= file->moduleSize) {
				fprintf(stderr, "Invalid address on symbol: %s\n",temp->label);
				return -1;
			}
			array[temp->address] = temp->value;
		}
		temp = temp->next;
	}
	int i, command = 0;
	char word[MAX], label[MAX], val[MAX];
	for (i = 0; i < file->lines; ++i) {
		sscanf(file->array[i], "%s %s", word, val);
		if (isInstruction(word)) {
			uint32_t bytes = convertIntoBytes(word,val, file->symbolList);
			((uint32_t*)array)[command++] = bytes;
		}
		if (sscanf(file->array[i], "%s %s %s", label, word, val) != 3)
		;//error
		if (!isInstruction(label) && isInstruction(word)) {
			uint32_t bytes = convertIntoBytes(word,val,file->symbolList);
			((uint32_t*)array)[command++] = bytes;
		}

	}
	// TODO: actually write the array to a file

	// print the array
	printf("Binary:\n");
	for (i = 0; i < file->moduleSize; ++i) {
		printf("%04x ", array[i]);
		if (((i + 1) % 4) == 0) printf("\n");
	}
	printf("\n");

	// free array
	free(array);
	return 0;
}

int countLines (FILE* fh) {
	if (fh == NULL) return -1;
	int lines = 0;
	while (!feof(fh)) {
		lines += nextLine(fh);
	}
	return lines;
}

int nextLine(FILE* fh) {
	int ch = fgetc(fh);
	// Strip whitespaces and tabs from beginning
	while (ch != EOF && (ch == ' ' || ch == '\t')) ch = fgetc(fh);
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
		input[i] = (char*) malloc(MAX * sizeof(char));
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
uint32_t convertIntoBytes(char* word, char* val, label_list* symbols) {
	// do nothing

	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"

int countLines(FILE*);
int nextLines(FILE*);
char** readCode(FILE*, int);

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

int countLines (FILE* fh) {
	if (fh == NULL) return -1;
	int lines = 0;
	int ch = 0;	
	while (!feof(fh)) {
		lines += nextLine(fh);
	}
	return lines;
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

#include <stdio.h>
#include <stdlib.h>


int countLines(FILE*);

int main (int argc, char* argv[]) {

	if (argc < 2) return -1;

	// we take the first argument to be name of a .ttk91 file
	char* filename = argv[1];

	printf("file is : %s\n",filename);

	// open file and count the lines but not comments
	FILE* fh = fopen(filename,"r");
	int lines = countLines(fh);
	printf("lines = %d\n",lines);
	if (lines == -1) return -1;
	rewind(fh);
	
	// reserve space and read the code lines from the file
	char** input = (char**) malloc(lines*sizeof(char*));
	for (int i = 0; i < lines; ++i) {
		if (feof(fh)) {
			printf("Error reading the source file!\n");
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
	fclose(fh);

	// free space for the file array
	for (int i = 0; i < lines; ++i) {
		free(input[i]);
	}
	free(input);
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

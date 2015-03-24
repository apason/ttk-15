#include <stdio.h>
#include <stdlib.h>


int countLines(FILE*);

int main (int argc, char* argv[]) {

	// we take the first argument to be name of a .ttk91 file
	char* file;
	if (argc > 1) 	file = argv[1];
	else		return -1;

	// open file and count the lines but not comments
	FILE* fh = fopen(file,"r");
	int lines = countLines(fh);
	rewind(fh);
	
	// reserve space and read the code lines from the file
	char** input = (char**) malloc(lines*sizeof(char*));
	for (int i = 0; i < lines; ++i) {
		if (feof(fh)) {
			printf("Error reading the source file!\n");
			return -1;
		}
		input[i] = (char*) malloc(256 * sizeof(char*));
		int ch = fgetc(fh), count = 0;
		while (ch == ' ' || ch == '\t') ch = fgetc(fh);
		if (ch == ';') {
			while (fgetc(fh) != '\n');
			ch = fgetc(fh);
			while (ch == ' ' || ch == '\t') ch = fgetc(fh);
		}
		while (ch != '\n') {
			if (ch == ';') {
				while (fgetc(fh) != '\n');
				continue;
			}
			*input[count++] = ch;
		}
		*input[count] = 0;
	}

	// free space for the file array
	for (int i = 0; i < lines; ++i) {
		free(input[i]);
	}
	free(input);
	return 0;
}

int countLines (FILE* fh) {
	if (fh == NULL) return -1;
	int lines = 0;
	int ch = 0;	
	while (!feof(fh)) {
		ch = fgetc(fh);
		while (ch == ' ' || ch == '\t') ch = fgetc(fh);
		if (ch == ';') continue;
		while (ch != '\n') fgetc(fh);
		++lines;
	}
	return lines;
}

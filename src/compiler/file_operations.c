#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <compiler.h>

int countLines(FILE*);
int nextLine(FILE*);
char** readCode(FILE*, int);
void writeInstruction(char*,char*,label_list*, FILE*);

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
	// TODO: dynamic name for the object file
	FILE* fh = fopen("testi.o15","wb");
	if (fh == NULL) {
		fprintf(stderr, "Error opening file!!!!!\n");
		return -1;
	}
	// write header to the object file
	uint32_t dataSegmentAddress = file->codeSize*5 + 8;
	uint32_t symbolTableAddress =  dataSegmentAddress + (file->moduleSize - file->codeSize)*4;
	fwrite(&symbolTableAddress,sizeof(uint32_t),1,fh);
	fwrite(&dataSegmentAddress, sizeof(uint32_t),1,fh);
	// start writing data
	int i, cInstructions = 0;
	char word[MAX], label[MAX], val[MAX];
	for (i = 0; i < file->lines; ++i) {
		sscanf(file->array[i], "%s %s", word, val);
		if (isInstruction(word)) {
			writeInstruction(word,val,file->symbolList, fh);
			++cInstructions;
		}
		if (sscanf(file->array[i], "%s %s %s", label, word, val) != 3)
		;//error
		if (!isInstruction(label) && isInstruction(word)) {
			writeInstruction(word,val,file->symbolList, fh);
			++cInstructions;
		}

	}

	label_list* symbols = file->symbolList;
	while (symbols!=NULL) {
		if (symbols->size == 1) {
			fwrite(&(symbols->value),sizeof(symbols->value),1,fh);
		} else if (symbols->size > 1) {
			for (i = 0; i < symbols->size; ++i) {
				uint32_t nul = 0;
				fwrite(&nul, 1, sizeof(uint32_t),fh);
			}
		}
		symbols = symbols->next;	
	}
	symbols = file->symbolList;
	while (symbols != NULL) {
		if (symbols->size >= 0) {
			fwrite(symbols->label,sizeof(char),32,fh);
			fwrite(&(symbols->address),sizeof(symbols->address),1,fh);
		}
		symbols = symbols->next;
	}
	fclose(fh);
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
void writeInstruction(char* word,char* val,label_list* symbols, FILE* fh) {
	// DO NOTHING
	static const char opcodes[38][8] = {\
		"nop\0\0\0\0",\
		"store\0\x1",\
		"load\0\0\0\x2",\
		"in\0\0\0\0\x3",\
		"out\0\0\0\x4",\
		"add\0\0\0\x11",\
		"sub\0\0\0\x12",\
		"mul\0\0\0\x13",\
		"div\0\0\0\x14",\
		"mod\0\0\0\x15",\
		"and\0\0\0\x16",\
		"or\0\0\0\0\x17",\
		"xor\0\0\0\x18",\
		"shl\0\0\0\x19",\
		"shr\0\0\0\x1A",\
		"not\0\0\0\x1B",\
		"shra\0\0\x1C",\
		"comp\0\0\x1F",\
		"jump\0\0\x20",\
		"jneg\0\0\x21",\
		"jzer\0\0\x22",\
		"jpos\0\0\x23",\
		"jnneg\0\x24",\
		"jnpos\0\x25",\
		"jles\0\0\x27",\
		"jequ\0\0\x28",\
		"jgre\0\0\x29",\
		"jnles\0\x2A",\
		"jnequ\0\x2B",\
		"jngre\0\x2C",\
		"call\0\0\x31",\
		"exit\0\0\x32",\
		"push\0\0\x33",\
		"pop\0\0\0\x34",\
		"pushr\0\x35",\
		"popr\0\0\x36",\
		"svc\0\0\0\x70" };
	uint32_t instruction = 0;
	int i;
	printf("%d\n",opcodes[1][6]);
	for (i = 0; i < 38; ++i) {
		if (strncmp(opcodes[i],word,strlen(word)) == 0) {
			instruction |= (opcodes[i][6] << 24);
			break;
		}
	}
	char* argument = strchr(val, ',');
	*argument++ = '\0';

	if (tolower(val[0]) == 'r' && isdigit(val[1]) && val[2] == 0) {
		int reg = atoi(val + 1);
		if (reg > 7); //error
		instruction |= (reg << 21);
	} else if (tolower(val[1]) == 'p' && val[2] == 0) {
		if (tolower(val[0] == 's'))
			instruction |= 6 << 21;
		else if (tolower(val[0] == 'f'))
			instruction |= 7 << 21;
		else
		;//error
	} else
	;//error
	
	// make the argument lowercase
	char* p = argument;
	for (; *p; ++p) *p = tolower(*p);
	// see if the label we have here matches our records
	label_list* temp = symbols;
	while(temp != NULL) {
		if (!strncmp(temp->label,argument,strlen(argument))) {
			instruction |= temp->address;
			break;
		}
		// TODO: add foreign labels to list as external
		temp = temp->next;
	}
	uint8_t firstByte;
	if (temp->address < 0) {
		firstByte = 2;
	} else if (temp->size > 0) {
		firstByte = 1;
	} else {
		firstByte = 0;
	}
	// write a byte indicating whether the address is hardcoded, local or external to the module
	fwrite(&firstByte,sizeof(firstByte),1,fh);
	// write the compiled instruction
	fwrite(&instruction,sizeof(instruction),1,fh);
}


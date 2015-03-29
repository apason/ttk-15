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
	printf("Opening output file: %s\n",file->out_name);
	FILE* fh = fopen(file->out_name,"wb");
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
	char word[MAX], label[MAX], val[MAX] = "\0";
	for (i = 0; i < file->lines; ++i) {
		sscanf(file->array[i], "%s %s", word, val);
		if (isInstruction(word)) {
			writeInstruction(word,val,file->symbolList, fh);
			++cInstructions;
			continue;
		}
		if (sscanf(file->array[i], "%s %s %s", label, word, val) != 3)
			fprintf(stderr,"Error reading line: %d\n",cInstructions);//error
		if (!isInstruction(label) && isInstruction(word)) {
			writeInstruction(word,val,file->symbolList, fh);
			++cInstructions;
		}

	}
	printf("Starting to write the symbol list\n");
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
			return NULL;
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

int getRegister(char* val, int errors) {

	int reg = 0;
	if (tolower(val[0]) == 'r' && isdigit(val[1]) && val[2] == 0) {
		reg = atoi(val + 1);
		if (reg > 7) {
			if (errors) fprintf(stderr, "Invalid register r%d\n",reg);
			return -1;
		}
	} else if (tolower(val[1]) == 'p' && val[2] == 0) {
		if (tolower(val[0] == 's'))
			reg = 6;
		else if (tolower(val[0] == 'f'))
			reg = 7;
		else {
			if (errors) fprintf(stderr, "First operand must be a register, found: %s\n",val);
			return -1;
		}
	} else {
		if (errors) fprintf(stderr, "First operand must be a register, found: %s\n",val);
		return -1;
	}
	return reg;
}


void writeInstruction(char* word,char* val,label_list* symbols, FILE* fh) {
	static const char opcodes[38][8] = {\
		"nop\0\0\0\0",\
		"store\0\x1",\
		"load\0\0\x2",\
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
	// nop is just zeros
	if (!strncmp(word,"nop",3)) {
		fwrite(&instruction,sizeof(uint8_t),1,fh);
		fwrite(&instruction,sizeof(instruction),1,fh);
		return;
	}
	// find out the operation code
	for (i = 1; i < 38; ++i) {
		if (strncmp(opcodes[i],word,strlen(opcodes[i])) == 0) {
			instruction |= (opcodes[i][6] << 24);
			break;
		}
		if (i == 37) {
			fprintf(stderr,"Unknown opcode: %s\n",word);
			return;
		}
	}
	char* argument = strchr(val, ',');
	if (argument != NULL && strlen(argument) > 1)
		*argument++ = '\0';
	else {
		argument = val;
	}
		

	// figure out which indexind mode to use
	uint32_t mode = 1;
	if (argument[0] == '=') {
		if (argument[1] == '\0') {
			fprintf(stderr, "Invalid argument: %s\n", argument);
			return;
		}
		mode = 0;
		++argument;
	} else if (argument[0] == '@') {
		if (argument[1] == '\0') {
			fprintf(stderr, "Invalid argument: %s\n", argument);
			return;
		}
		mode = 2;
		++argument;
	}

	// set the indexing mode
	instruction |= mode << 19;
	
	// set Register
	int reg = 0;
	if (argument > val) {
		reg = getRegister(val, 1);
		if (reg < 0) return;
	}
	instruction |= reg << 21;

	// make the argument lowercase
	{ char* p = argument;
	for (; *p; ++p) *p = tolower(*p); }

	// see if the label we have here matches our records
	label_list* temp = symbols;

	if ((reg = getRegister(argument, 0)) > 0) {
		instruction |= reg;
		temp = NULL;
	// if argument is kbd, set address is 1
	} else if (!strncmp("kbd",argument,strlen(argument))) {
		instruction |= 1;
		temp = NULL;
	} else if (!strncmp("halt",argument,strlen(argument))) {
		instruction |= 0xB;
		temp = NULL;
	} else if (isdigit(argument[0])) {
		instruction |= atoi(argument);
		temp = NULL;
	} else if (strncmp("crt",argument,strlen(argument))) {
		int16_t index = -1;
		while(temp != NULL) {
			// when label is found it's address is replaced in the instruction
			if (!strncmp(temp->label,argument,strlen(argument))) {
				instruction |= temp->address;
				// make sure equ are not treated as labels
				if (temp->size < 0) temp = NULL;
				break;
			}
			// don't use the same index as the other external labels
			if (temp->address < 0) --index;
			temp = temp->next;
		}
		if (temp == NULL) {
			// Didn't find label so added to the list of external symbols
			temp = symbols;
			while (temp->next != NULL) temp = temp->next;
			temp->next = (label_list*)malloc(sizeof(label_list));
			temp = temp->next;
			temp->next = NULL;
			strncpy(temp->label,argument,strlen(argument));
			temp->label[strlen(argument)] = '\0';
			temp->size = 0;
			temp->address = index;
			instruction |= index;
		}
	}

	// set info on whether there is a label or not in the address field of the instruction
	uint8_t firstByte;
	if (temp != NULL) {
		// there is a label
		firstByte = 1;
	} else {
		// no label
		firstByte = 0;
	}
	// write a byte indicating whether the address is hardcoded, local or external to the module
	fwrite(&firstByte,sizeof(firstByte),1,fh);
	// write the compiled instruction
	fwrite(&instruction,sizeof(instruction),1,fh);
}


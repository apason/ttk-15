#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <compiler.h>
#include <ttk-15.h>

int countLines(FILE*);
int nextLine(FILE*);
char** readCode(FILE*, int);
void writeInstruction(char* instr,char* val,label_list* symbols, FILE*,int);
void freeSymbols(code_file*);
void freeCodeArray(code_file*);

int getHardcodedSymbolValue(char* argument);
int getOpCode(char* operation);
int getRegister(char* argument, int errors);
int getIndexingMode(char* argument);
int getIndexRegister(char* argument);
int getAddress(char* argument, label_list* symbols, uint8_t* firstByte);

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
		fprintf(stderr, "Error linecount <= 0, is it an empty file?\n");
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
	MYTYPE dataSegmentAddress = file->codeSize*5 + 8;
	MYTYPE symbolTableAddress =  dataSegmentAddress + (file->moduleSize - file->codeSize)*4;
	fwrite(&symbolTableAddress,sizeof(MYTYPE),1,fh);
	fwrite(&dataSegmentAddress, sizeof(MYTYPE),1,fh);
	// start writing data
	int i, cInstructions = 0;
	char word[MAX], label[LABELLENGTH], val[MAX];
	for (i = 0; i < file->lines; ++i) {
		val[0] = '\0';
		sscanf(file->array[i], "%s %s", word, val);
		if (isInstruction(word)) {
			writeInstruction(word,val,file->symbolList, fh, file->ttk_15);
			++cInstructions;
			continue;
		}
		val[0] = '\0';
		if (sscanf(file->array[i], "%s %s %s", label, word, val) < 2)
			fprintf(stderr,"Error reading line: %d\n",cInstructions);//error
		if (!isInstruction(label) && isInstruction(word)) {
			writeInstruction(word,val,file->symbolList, fh, file->ttk_15);
			++cInstructions;
		}

	}
	printf("Starting to write the symbol list\n");
	label_list* symbols = file->symbolList;
	while (symbols!=NULL) {
		if (symbols->size == 1) {
			fwrite(&(symbols->value),sizeof(MYTYPE),1,fh);
		} else if (symbols->size > 1) {
			for (i = 0; i < symbols->size; ++i) {
				MYTYPE nul = 0;
				fwrite(&nul, 1, sizeof(MYTYPE),fh);
			}
		}
		symbols = symbols->next;	
	}
	symbols = file->symbolList;
	while (symbols != NULL) {
		if (symbols->size >= 0) {
			if (strlen(symbols->label)>32)
				fprintf(stderr, "Warning symbol name more than 32 chars, will be cut: %s\n",symbols->label);
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
			if (ch == ',')
				while ((ch = fgetc(fh)) == ' ' || ch == '\t');
			else
				ch = fgetc(fh);
		}
		input[i][count] = 0;
		printf("Line %d: %s\n",i+1,input[i]);
	
	}
	return input;
}



void writeInstruction(char* word,char* val,label_list* symbols, FILE* fh, int ttk_15) {

	uint8_t firstByte = 0;
	uint32_t instruction = 0;

	int opCode = 0;
	int reg = 0;
	int mode = 0;
	int ireg = 0;
	int addr = 0;

	int nargs = 0;

	// find out the operation code
	opCode = getOpCode(word);
	if (opCode == -1) return;

	// find number of arguments
	char* argument = NULL;
	if (val[0]) {
		// make the arguments lowercase
		{
			char* p = val;
			for (; *p; ++p) *p = tolower(*p);
		}
		// split val into two if there's a comma
		argument = strchr(val, ',');
		if (argument != NULL && strlen(argument) > 1) {
			*argument++ = '\0';
			nargs = 2;
		} else {
			nargs = 1;
			argument = val;
		}
	} else
		nargs = 0;

	// if we have arguments for the instruction
	if (nargs) {
		int temp;
		// get indexing mode
		if ((temp = getIndexingMode(argument)) < 0)
			return;
		if (temp != 1)
			++argument;
		mode = temp;
		// get index register
		if ((ireg = getIndexRegister(argument)) < 0)
			return;
		// two arguments, first one has to be a register
		if (nargs == 2) {
			if ((reg = getRegister(val, 1)) < 0)
				return;
			// check if opcode is store
			if (opCode == 1 && !ttk_15) {
				if (--mode < 0) {
					fprintf(stderr, "invalid mode %d on store! %s\n",mode,argument);
					return;
				}
			}
		}
		if (( temp = getRegister(argument, 0)) >= 0)
			if (nargs == 2) {
				ireg = temp;
				if (--mode < 0) {
					fprintf(stderr, "invalid mode!\n");
					return;
				}
			} else
				reg = temp;
		else
			addr = getAddress(argument, symbols, &firstByte);
		
	}

	
	instruction |= (opCode << 24);
	instruction |= (reg << 21);
	instruction |= (mode << 19);
	instruction |= (ireg << 16);
	instruction |= (addr & 0xffff);

	// write 1, if address is a label, 0, if not
	fwrite(&firstByte,sizeof(firstByte),1,fh);
	// write the compiled instruction
	fwrite(&instruction,sizeof(instruction),1,fh);
}

void freeCodeFile(code_file* file) {
	freeSymbols(file);
	freeCodeArray(file);
}

void freeSymbols(code_file* file) {
	while (file->symbolList != NULL) {
		label_list* temp = file->symbolList;
		file->symbolList = file->symbolList->next;
		free(temp);
	}
}

void freeCodeArray(code_file* file) {
	int i;
	for (i = 0; i < file->lines; ++i) {
		free(file->array[i]);
	}
	free(file->array);
}

int getHardcodedSymbolValue(char* arg) {
	if (!strncmp(arg, "crt", 4))
		return 0x0;
	else if (!strncmp(arg, "kbd", 4))
		return 0x1;
	else if (!strncmp(arg, "stdin", 6))
		return 0x6;
	else if (!strncmp(arg, "stdout", 7))
		return 0x7;
	else if (!strncmp(arg, "halt", 5))
		return 0xB;
	else if (!strncmp(arg, "read", 5))
		return 0xC;
	else if (!strncmp(arg, "write", 6))
		return 0xD;
	else if (!strncmp(arg, "time", 5))
		return 0xE;
	else if (!strncmp(arg, "date", 5))
		return 0xF;
	return -1;
}

int getOpCode(char* word) {
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
	int i;
	for (i = 0; i < 38; ++i) {
		if (strncmp(opcodes[i],word,strlen(opcodes[i])) == 0) {
			return opcodes[i][6];
		}
	}
	fprintf(stderr,"Unknown opcode: %s\n",word);
	return -1;
}


int getIndexingMode(char* argument) {
	if (argument[0] == '=') {
		if (argument[1] == '\0') {
			fprintf(stderr, "Invalid argument: %s\n", argument);
			return -1;
		}
		return 0;
	} else if (argument[0] == '@') {
		if (argument[1] == '\0') {
			fprintf(stderr, "Invalid argument: %s\n", argument);
			return -1;
		}
		return 2;
	}
	else
		return 1;
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

int getIndexRegister(char* argument) {

	// figure if there is a need for indexing register
	char* bracket = argument;
	while (*bracket && (*bracket != '(')) ++bracket;
	if (*bracket == '(') {
		// found braces, inside must be a register
		*bracket++ = '\0';
		char* p = bracket;
		while (*p && *p != ')') ++p;
		if (*p == ')') {
			*p = '\0';
			return getRegister(bracket,1);
		} else {
			fprintf(stderr, "Missing closed braces!\n");
			return -1;
		}
	}
	return 0;
}

int getAddress(char* argument, label_list* symbols, uint8_t *firstByte) {
	label_list* temp = symbols;
	int addr;
	// is it a number?
	if (isdigit(argument[0]) || (isdigit(argument[1]) && argument[0] == '-')) {
		addr = atoi(argument);
	// is it not a hardcoded symbol, but a label?
	} else if ((addr = getHardcodedSymbolValue(argument)) < 0) {
		*firstByte = 1;
		int16_t index = -1;
		while(temp != NULL) {
			// when label is found it's address is replaced in the instruction
			if (!strncmp(temp->label,argument,strlen(argument))) {
				addr = temp->address;
				// make sure equ are not treated as labels
				if (temp->size < 0) *firstByte = 0;
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
			// zero the label char array
			memset(temp->label,0,sizeof(temp->label));
			strncpy(temp->label,argument,LABELLENGTH);
			temp->size = 0;
			temp->address = (int16_t)index;
			addr = index;
		}
	}
	return addr;
}

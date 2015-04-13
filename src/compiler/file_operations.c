#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <ttk-15.h>
#include "errorcodes.h"
#include "compiler.h"

static int countLines(FILE*);
static int nextLine(FILE*);
static char** readCode(FILE*, int, int**);
static int writeInstruction(char* instr,char* val,label_list* symbols, FILE*,int);
static void freeSymbols(code_file*);
static void freeCodeArray(code_file*);
static void print_error(int error, int line);


// this is a function to be used outside this file
int readCodeFile(code_file* file) {
	FILE* fh = fopen(file->name,"r");
	if (fh == NULL) {
		fprintf(stderr, "Error opening file: %s\n",file->name);
		return -1;
	}
	
	//printf("file is : %s\n",file->name);
	
	if ((file->lines = countLines(fh)) <= 0) {
		fclose(fh);
		fprintf(stderr, "Error linecount <= 0, is it an empty file?\n");
		return -1;
	}
	//printf("lines = %d\n",file->lines);
	
	rewind(fh);

	// read code lines to an array
	file->array = readCode(fh, file->lines,&(file->code_text));

	fclose(fh);
	return 0;
}

static char** readCode(FILE* fh, int lines, int**ppcode_text) {
	int i;
	int text_lines = 0;
	int* code_text = (int*) malloc((lines + 1)*sizeof(int));
	*ppcode_text = code_text;
	// reserve space and read the code lines from the file
	char** input = (char**) malloc(lines*sizeof(char*));
	for (i = 0; i < lines; ++i) {
		if (feof(fh)) {
			fprintf(stderr, "Error reading the source file!\n");
			return NULL;
		}
		int ch = fgetc(fh);

		// Skip comment lines and whitespaces
		do {
			while (isspace(ch)) {
				if (ch == '\n') ++text_lines;
				ch = fgetc(fh);
			}
			if (ch == ';') {
				++text_lines;
				while (fgetc(fh) != '\n');
				ch = fgetc(fh);
			}
		} while (ch == ';' || ch == '\n');

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
				while (isspace(ch = fgetc(fh)));
			else
				ch = fgetc(fh);
		}
		input[i][count] = 0;
		code_text[i] = ++text_lines;
		//printf("Line %d: %s\n",i+1,input[i]);
	
	}
	return input;
}

static int countLines (FILE* fh) {
	if (fh == NULL) return -1;
	int lines = 0;
	while (!feof(fh)) {
		lines += nextLine(fh);
	}
	return lines;
}

static int nextLine(FILE* fh) {
	int ch = fgetc(fh);
	// Strip whitespaces from beginning
	while (ch != EOF && isspace(ch)) ch = fgetc(fh);
	// If this line is a comment, let's skip it
	if (ch == ';') {
		while ((ch = fgetc(fh)) != EOF && ch != '\n');
		return 0;
	}
	// If this line is empty, skip it too
	if (ch == '\n') {
		return 0;
	}
	// If we have reached end of file, there is no line
	if (ch == EOF) return 0;
	// Valid line so seek through and add 1 to line count
	while (ch != EOF && ch != '\n') ch = fgetc(fh);
	return 1;
}


// this is also a function to be used outside of this file
int writeCodeFile(code_file* file) {
	int error = 0;
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
			error = writeInstruction(word,val,file->symbolList, fh, file->ttk_15);
			// print error if found
			if (error < 0) {
				print_error(error, file->code_text[cInstructions]);
				fprintf(stderr,"\t\"%s\"\n",file->array[i]);
			}
			++cInstructions;
			continue;
		}
		val[0] = '\0';
		if (sscanf(file->array[i], "%s %s %s", label, word, val) < 2) {
			fprintf(stderr,"Error reading line: %d\n",file->code_text[cInstructions]);//error
			fprintf(stderr,"\t\"%s\"\n",file->array[i]);
		}
		if (!isInstruction(label) && isInstruction(word)) {
			error = writeInstruction(word,val,file->symbolList, fh, file->ttk_15);
			// print error if found
			if (error < 0) {
				print_error(error, file->code_text[cInstructions]);
				fprintf(stderr,"\t\"%s\"\n",file->array[i]);
			}
			++cInstructions;
		}

	}
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
				fprintf(stderr, "Warning: symbol name more than 32 chars, will be cut: %s\n",symbols->label);
			fwrite(symbols->label,sizeof(char),32,fh);
			fwrite(&(symbols->address),sizeof(symbols->address),1,fh);
		}
		symbols = symbols->next;
	}
	fclose(fh);
	return 0;
}


static int writeInstruction(char* word,char* val,label_list* symbols, FILE* fh, int ttk_15) {

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
	if (opCode == -1) return INVALIDOPCODE;

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
			return INVALIDMODE;
		if (temp != 1)
			++argument;
		mode = temp;
		// get index register
		if ((ireg = getIndexRegister(argument)) < 0)
			return INVALIDIREG;
		// two arguments, first one has to be a register
		if (nargs == 2) {
			if ((reg = getRegister(val, 1)) < 0)
				return INVALIDREG;
			// check if opcode is store
			if (opCode == 1 && !ttk_15) {
				if (--mode < 0)
					return INVALIDMODE;
			}
		}
		if (( temp = getRegister(argument, 0)) >= 0)
			if (nargs == 2) {
				ireg = temp;
				if (--mode < 0)
					return INVALIDMODE;
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
	return 0;
}

void freeCodeFile(code_file* file) {
	freeSymbols(file);
	freeCodeArray(file);
}

static void freeSymbols(code_file* file) {
	while (file->symbolList != NULL) {
		label_list* temp = file->symbolList;
		file->symbolList = file->symbolList->next;
		free(temp);
	}
}

static void freeCodeArray(code_file* file) {
	int i;
	for (i = 0; i < file->lines; ++i) {
		free(file->array[i]);
	}
	free(file->array);
	free(file->code_text);
}

static void print_error(int error, int line) {
	switch(error) {
		case INVALIDOPCODE:
			fprintf(stderr,"Invalid operation code on line: %d\n",line);
			break;
		case INVALIDMODE:
			fprintf(stderr,"Invalid mode on line: %d\n",line);
			break;
		case INVALIDIREG:
			fprintf(stderr,"Invalid index register on line: %d\n",line);
			break;
		case INVALIDREG:
			fprintf(stderr,"Invalid register on line: %d\n",line);
			break;
		default:
			fprintf(stderr,"Unknown syntax error on line: %d\n",line);
	}
}

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <compiler.h>

int countSize(code_file* file){
  file->symbolList = (label_list*)malloc(sizeof(label_list));
  label_list* label_node = file->symbolList;
  char **code = file->array;
  label_list* first = label_node;

  int i;
  int size=0;
  char word[MAX], label[MAX], val[MAX];

  label_node->size = 0;
  label_node->label[0] = '\0';
  label_node->next = NULL;

  for(i = 0; i < file->lines; i++){
    sscanf(code[i], "%s %s", word, val);
    if(isInstruction(word))
      ++size;
    else{
      if(sscanf(code[i], "%s %s %s", label, word, val) != 3){
	printf("invalid start of expression: %s\n", label);
	exit(-1);
      }
	
      if(!isInstruction(label) && isInstruction(word))
	label_node->address = size++;
      else if(!strncmp(word, "dc", MAX)) {
	 label_node->size = 1;
	// store the value to be inserted later
	 label_node->value = atoi(val);
	}
      else if(!strncmp(word, "ds", MAX)) {
	// remember the size
	label_node->size = atoi(val);
	}
      else if(!strncmp(word, "equ", MAX)) {
	// now address/label is inserted into label
	label_node->address = atoi(val);
	label_node->size = -1;
	}
      else{
	printf("ERROR: invalid instruction %s\n", word);
	exit(-1);
      }

      // set label name
      strncpy(label_node->label, label, MAX);
      // reserve space for next label
      label_node->next = (label_list*) malloc(sizeof(label_list));
      label_node = label_node->next;
      label_node->size = 0;
      label_node->next = NULL;
      label_node->label[0] = '\0';
    }
  }
  label_node = first;
  file->codeSize = size;
  // calculate addresses add the variables to the end of code
  while(label_node->next != NULL) {
	// the dc case
	if (label_node->size == 1) {
		label_node->address = size++;
	// the ds case
	} else if (label_node->size > 1) {
		label_node->address = size;
		size += label_node->size;
	}
	label_node = label_node->next;
  }
  file->moduleSize = size;
  while (first->next != NULL && first->next->next != NULL) {
	first = first->next;
  }
  free(first->next);
  first->next = NULL;
  
  return 0;
}

int isInstruction(char *word){
  char *p = word;
  // make word lowercase
  for (; *p; ++p) *p = tolower(*p);
  int i;
  char instructions[38][6] = {"nop", "store", "load", "in", "out",\
                              "add", "sub", "mul", "div", "mod",\
                              "and", "or", "xor", "shl", "shr", "not", "shra",\
                              "comp", "jump", "jneg", "jzer", "jpos", "jnneg", "jnzer", "jnpos",\
                              "jles", "jequ", "jgre", "jnles", "jnequ", "jngre",\
                              "call", "exit", "push", "pop", "pushr", "popr", "svc"};
  for(i = 0; i < 38; i++)
    if(!strncmp(instructions[i], word, MAX))
      return 1;
  return 0;

}


#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX 128

typedef struct label_list {
	struct label_list* next;
	char[MAX] label;
	int address;
	int size;
} label_list;

static int isInstruction(char *word);

int countSize(char **code, int length, label_list* label_node){
  if (label_node == NULL) {
    fprintf(stderr, "label_node should not be null\n");
    return -1;
  }
  label_node->size = 0;
  int i;
  int size=0;
  char word[MAX], label[MAX], val[MAX];
  for(i = 0; i < length, i++){
    sscanf(code[i], "%s %s", word, val);
    if(isInstruction(word))
      size++;
    else{
      if(sscanf(code[i], "%s %s %s", label, word, val) != 3)
	;//error
      if(!isInstruction(label) && isInstruction(word))
	size++;
      if(!strncmp(word, "dc", MAX)) {
	 size++;
	 label_node->size = 1;
	// store the value to be inserted in the address member
	 label_node->address = atoi(val);
	}
      else if(!strncmp(word, "ds", MAX)) {
	size += atoi(val);
	// remember the size
	label_node->size = atoi(val);
	}
      else if(strncm(word, "equ", MAX)) {
	// now address/label is inserted into label
	label_node->address = atoi(val);
	label_node->size = -1;
	;//error
	}
	// set label name
	label_node->label = label;
	// reserve space for next label
	label_node->next = (label_list*) malloc(sizeof(label_list));
	label_node = label_node->next;
	label_node->size = 0;
    }
  }
  free(label_node->next);
  label_node->next = NULL;
  return size;
}

static int isInstruction(char *word){
  int i;
  char instructions[38][6] = {"nop", "store", "load", "in", "out",\
                              "add", "sub", "mul", "div", "mod",\
                              "and", "or", "xor", "shl", "shr", "not", "shra",\
                              "comp", "jump", "jneg", "jzer", "jpos", "jnneg", "jnzer", "jnpos",\
                              "jles", "jequ", "jgre", "jnles", "jnequ", "jngre",\
                              "call", "exit", "push", "pop", "pushr", "popr", "svc"};
  for(i = 0; i < 38; i++)
    if(strncmp(instructions[i], word, MAX))
      return 1;
  return 0;

}


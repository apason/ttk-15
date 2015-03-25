#include <stdio.h>
#include <stdlib.h>
#define MAX 128

typedef struct label_list {
	struct label_list* next;
	char[MAX] label;
	int address;
	int size;
} label_list;

int countSize(char **code, int length, label_list* label_node){
  if (label_node != NULL) {
    fprintf(stderr, "label_node should be null\n");
    return -1;
  }
  label_node = (label_list*)malloc(sizeof(label_list));
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
      if(!strncmp(word, "dc", MAX))
	 size++;
      else if(!strncmp(word, "ds", MAX)) {
	size += atoi(val);
	label_node->size = atoi(val);
	}
      else if(strncm(word, "equ", MAX)) {
	label_node->address = atoi(val);
	label_node->size = -1;
	;//error
	}
	label_node->label = label;
	label_node->next = (label_list*) malloc(sizeof(label_list));
	label_node = label_node->next;
	label_node->size = 0;
    }
  }
}


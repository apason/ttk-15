#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX 128

static int isInstruction(char *word);

int countSize(char **code, int length){
  int i;
  int size=0;
  char word[MAX], label[MAX], val[MAX];
  for(i = 0; i < length; i++){
    sscanf(code[i], "%s", word);
    if(isInstruction(word))
      size++;
    else{
      if(sscanf(code[i], "%s %s %s", label, word, val) != 3)
	;//error
      if(!isInstruction(label) && isInstruction(word))
	size++;
      if(!strncmp(word, "dc", MAX))
	 size++;
      else if(!strncmp(word, "ds", MAX))
	size += atoi(val);
      else if(strncm(word, "equ", MAX))
	;//error	      
    }
  }
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


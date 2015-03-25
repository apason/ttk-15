#define MAX 128

int countSize(char **code, int length){
  int i;
  int size=0;
  char word[MAX], label[MAX], val[MAX];
  for(i = 0; i < length, i++){
    sscanf(code[i], "%s", word);
    if(isInstruction(word))
      size++;
    else{
      if(sscanf(code[i]m "%s %s %s", label, word, val) != 3)
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


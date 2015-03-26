#ifndef COMPILER_COUNT_SIZE
#define COMPILER_COUNT_SIZE

typedef struct label_list {
	struct label_list* next;
	char label[128];
	int address;
	int size;
} label_list;

int countSize(char**, int, label_list*);
#endif 

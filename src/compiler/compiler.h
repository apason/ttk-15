typedef struct label_list {
	struct label_list* next;
	char[32] label;
	int address;
} label_list;

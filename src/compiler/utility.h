int getHardcodedSymbolValue(char* argument);
int getOpCode(char* operation);
int getRegister(char* argument, int errors);
int getIndexingMode(char* argument);
int getIndexRegister(char* argument);
int getAddress(char* argument, label_list* symbols, uint8_t* firstByte);

#include "test.h"



int main (int argc, char** argv) {
	int i;
	for (i = 0; i < NTESTS; ++i) {
		(*TEST[0])();
	}
	return 0;
}

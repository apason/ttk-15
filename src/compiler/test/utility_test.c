#include "../compiler.h"


void test_getindexingmode () {
	printf("Testing getIndexingMode()\n");
	int n = 3;
	int fail = 0;
	char* tests[] = {
		"=hello", (char*)0,
		"hello", (char*)1,
		"@hello", (char*)2
		};
	int i;
	for ( i = 0; i < n; i += 2) {
		int result = getIndexingMode(tests[i]);
		if (result != (int)tests[i + 1]) {
			fprintf(stderr, "getIndexingMode(%s), expected %d, got %d",tests[i], (int)tests[i + 1], result);
			fail = 1;
		}
	}
	if (fail)
		printf("\tFAILED\n");
	else
		printf("\tSUCCESS\n");
}

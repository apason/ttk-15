#include <stdio.h>
#include <stdarg.h>
#include "error.h"

int errno_linker;

void printError(void *asd,...){
    va_list vl;
    
    switch (errno_linker){
    case -6:
	va_start(vl, 1);
	fprintf(stderr, "ERROR: invalid label type on module %s\n", va_arg(vl, char*));
	va_end(vl);
	break;
    case -5:
	//NOT TESTED
	va_start(vl, 1);
	fprintf(stderr, "ERROR: invalid import table on module %s\n", va_arg(vl, char*));
	va_end(vl);
	break;
    case -4:
	va_start(vl, 2);
	fprintf(stderr, "ERROR: on module %s: ", va_arg(vl, char*));
	fprintf(stderr, "undefined label %s\n", va_arg(vl, char*));
	va_end(vl);
	break;
	//DOES NOT WORK
    case -3:
	va_start(vl, 2);
	fprintf(stderr, "ERROR: on module %s: ", va_arg(vl, char*));
	fprintf(stderr, "label %s defined in multiple modules\n", va_arg(vl, char*));
	va_end(vl);
	break;
    case -2:
	fprintf(stderr, "ERROR: no main module found\n");
	break;
    case -1:
	fprintf(stderr, "ERROR: several main modules detected\n");
	break;
    case 1:
	va_start(vl, 1);
	fprintf(stderr, "ERROR on module %s: object file is too small\n", va_arg(vl, char*));
	va_end(vl);
	break;
    case 2:
	va_start(vl,1);
	fprintf(stderr, "ERROR on module %s: incorrect object header\n", va_arg(vl, char*));
	va_end(vl);
	break;
    case 3:
	fprintf(stderr, "ERROR: no modules specified\n");
	break;
    }

}

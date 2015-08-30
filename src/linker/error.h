#ifndef error_header
#define error_header

//error codes

#define EINVALID_LABEL_TYPE     -7
#define EINVALID_EXPORT         -6
#define EINVALID_IMPORT         -5
#define ELABEL_NOT_FOUND        -4
#define EMULTIPLE_DECLARATIONS  -3
#define ENO_MAIN                -2
#define ESEVERAL_MAINS          -1
#define ELITTLE_OBJECT           1
#define EINVALID_HEADER          2
#define ENO_MODULES              3

extern void printError (void *, ...);

extern int errno_linker;


#endif


#ifndef ttk15
#define ttk15

#include <stdio.h>
#include <stdint.h>

#define MYTYPEF  float
#define MYTYPE   int32_t

//hardcoded symbols for machine
#define CRT    0x0
#define KBD    0x1
#define STDIN  0x6
#define STDOUT 0x7
#define HALT   0xB
#define READ   0xC
#define WRITE  0xD
#define TIME   0xE
#define DATE   0xF

#define LABELLENGTH  32 //only first 32 chars are counted!

//used in options mode and debug flag
#define UNDEFINED -1
#define TTK15     1
#define TTK91     0
#define ON         1
#define OFF        0

typedef uint16_t f16;

typedef struct ALU_UNIT{
    MYTYPE in1;
    MYTYPE in2;
    MYTYPE out;

} alu_unit;

typedef struct FPU_UNIT{
    MYTYPEF in1;
    MYTYPEF in2;
    MYTYPEF out;

} fpu_unit;

typedef struct MM_UNIT{
    MYTYPE limit;
    MYTYPE base;
    MYTYPE mbr;
    MYTYPE mar;

} mm_unit;

typedef struct CONTROL_UNIT{
    MYTYPE tr;
    MYTYPE ir;
    MYTYPE pc;
    MYTYPE sr;

} control_unit;

typedef struct MACHINE{
    MYTYPE        *regs;
    alu_unit      *alu;
    fpu_unit      *fpu;
    mm_unit       *mmu;
    control_unit  *cu;
    MYTYPE        *mem;
    long           memsize;

} machine;

//loader.c
extern int loadFile(MYTYPE *mem, FILE *file, MYTYPE memsize);
extern int loadFile91(MYTYPE *mem, FILE *file, MYTYPE memsize);

//float.h
extern f16 f16Encode(float f, int* error);
extern float f16Decode(f16 f);

#endif



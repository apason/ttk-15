#ifndef ttk15
#define ttk15

#include <stdio.h>

#include <stdint.h>

#define MYTYPE int32_t
#define FUNCTION(function) void function(machine *m, uint8_t rj, uint8_t mod, uint8_t ri, uint16_t mem)

//hardcoded symbols
#define CRT    0x0
#define KBD    0x1
#define STDIN  0x6
#define STDOUT 0x7
#define HALT   0xB
#define READ   0xC
#define WRITE  0xD
#define TIME   0xE
#define DATE   0xF

//used in options
#define UNDEFINED -1
#define TTK15      15
#define TTK91      91
#define ON         1
#define OFF        0

typedef struct ALU_UNIT{
  MYTYPE in1;
  MYTYPE in2;
  MYTYPE out;

} alu_unit;

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
  mm_unit       *mmu;
  control_unit  *cu;
  MYTYPE        *mem;
  long           memsize;

} machine;

typedef struct OPTIONS{
  FILE *file;
  int   mode;
  int   debug;
} options;

//prototype for machine instruction
typedef void(*instructionptr)(machine*, uint8_t, uint8_t, uint8_t, uint16_t);

//global variables
instructionptr instructions[255];
short mtl;

//options.c
extern options *getOptions(int argc, char *argv[]);

//machine.c
extern machine *newMachine(long memsize);
extern void startMachine(machine *m, int debug);

//mmu.c
extern void mmuGetData(mm_unit *mmu, MYTYPE *mem, MYTYPE x);
extern void mmuSetData(mm_unit *mmu, MYTYPE *mem, MYTYPE addr, MYTYPE data);

//cu.c
extern MYTYPE calculateSecondOperand(machine *m, uint8_t mode, uint8_t ri, int16_t addr);

//helpers.c
extern void initializeGlobals(void);
extern void printState(machine *m);
extern void freeMachine(machine *m);

//bitwise.c
extern uint8_t extractOpcode(MYTYPE x);
extern int16_t extractAddress(MYTYPE X);
extern uint8_t extractRj(MYTYPE x);
extern uint8_t extractMode(MYTYPE x);
extern uint8_t extractRi(MYTYPE x);

//loader.c
extern int loadFile(MYTYPE *mem, FILE *file);
extern int loadFile91(MYTYPE *mem, FILE *file);

//instructions.c
extern FUNCTION(nop);
extern FUNCTION(store);
extern FUNCTION(load);
extern FUNCTION(in);
extern FUNCTION(out);
extern FUNCTION(add);
extern FUNCTION(sub);
extern FUNCTION(mul);
extern FUNCTION(divv);
extern FUNCTION(mod);
extern FUNCTION(and);
extern FUNCTION(or);
extern FUNCTION(xor);
extern FUNCTION(shl);
extern FUNCTION(shr);
extern FUNCTION(shra);
extern FUNCTION(not);
extern FUNCTION(comp);
extern FUNCTION(jump);
extern FUNCTION(jneg);
extern FUNCTION(jzer);
extern FUNCTION(jpos);
extern FUNCTION(jnneg);
extern FUNCTION(jnzer);
extern FUNCTION(jnpos);
extern FUNCTION(jles);
extern FUNCTION(jequ);
extern FUNCTION(jgre);
extern FUNCTION(jnles);
extern FUNCTION(jnequ);
extern FUNCTION(jngre);
extern FUNCTION(call);
extern FUNCTION(exitt);
extern FUNCTION(push);
extern FUNCTION(pop);
extern FUNCTION(pushr);
extern FUNCTION(popr);
extern FUNCTION(svc);
#endif

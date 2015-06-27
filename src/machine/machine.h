#ifndef MACHINE
#define MACHINE

#include <stdio.h>
#include <stdint.h>

#define FUNCTION(function) void function(machine *m, uint8_t rj, uint8_t mod, uint8_t ri, uint16_t mem)

typedef struct OPTIONS{
  FILE  *file;
  int    mode;
  int    debug;
  MYTYPE memsize;

} options;

//prototype for machine instruction
typedef void(*instructionptr)(machine*, uint8_t, uint8_t, uint8_t, uint16_t);

//options.c
extern options *getOptions(int argc, char *argv[]);
extern void freeOptions(options *opts);

//machine.c
extern machine *newMachine(long memsize);
extern void startMachine(machine *m, int debug);
extern FUNCTION(haltMachine);

//mmu.c
extern void mmuGetData(mm_unit *mmu, MYTYPE *mem, MYTYPE x);
extern void mmuSetData(mm_unit *mmu, MYTYPE *mem, MYTYPE addr, MYTYPE data);

//utilities
extern void initializeInstructionArray(instructionptr *instructions);
extern void printState(machine *m);
extern void freeMachine(machine *m);

//instructions.c contains all machine instructions
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

/*
 * floating point extension
 */

extern FUNCTION(fin);
extern FUNCTION(fout);
extern FUNCTION(fadd);
extern FUNCTION(fsub);
extern FUNCTION(fmul);
extern FUNCTION(fdiv);
extern FUNCTION(fcomp);
extern FUNCTION(fjneg);
extern FUNCTION(fjzer);
extern FUNCTION(fjpos);
extern FUNCTION(fjnneg);
extern FUNCTION(fjnzer);
extern FUNCTION(fjnpos);

#endif

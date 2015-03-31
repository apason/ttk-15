#include <stdlib.h>
#include <stdio.h>

//project headers
#include <ttk-15.h>
#include <instructions.h>

//initializes all global variables excluded machine itself
void initializeGlobals(void){
  int i;

  // mytype length in bytes
  mtl = sizeof(MYTYPE) * 8; 

  for(i = 0; i < 255; i++) instructions[i] = NULL;

  instructions[NOP]   = nop;
  instructions[STORE] = store;
  instructions[LOAD]  = load;
  instructions[IN]    = in;
  instructions[OUT]   = out;
  instructions[ADD]   = add;
  instructions[SUB]   = sub;
  instructions[MUL]   = mul;
  instructions[DIV]   = divv;
  instructions[MOD]   = mod;
  instructions[AND]   = and;
  instructions[OR]    = or;
  instructions[XOR]   = xor;
  instructions[SHL]   = shl;
  instructions[SHR]   = shr;
  instructions[NOT]   = not;
  instructions[SHRA]  = shra;
  instructions[COMP]  = comp;
  instructions[JUMP]  = jump;
  instructions[JNEG]  = jneg;
  instructions[JZER]  = jzer;
  instructions[JPOS]  = jpos;
  instructions[JNNEG] = jnneg;
  instructions[JNZER] = jnzer;
  instructions[JNPOS] = jnpos;
  instructions[JLES]  = jles;
  instructions[JEQU]  = jequ;
  instructions[JGRE]  = jgre;
  instructions[JNLES] = jnles;
  instructions[JNEQU] = jnequ;
  instructions[JNGRE] = jngre;
  instructions[CALL]  = call;
  instructions[EXIT]  = exitt;    //exitt to not to be conflict with std function
  instructions[PUSH]  = push;
  instructions[POP]   = pop;
  instructions[PUSHR] = pushr;
  instructions[POPR]  = popr;
  instructions[SVC]   = svc;
}

//mainly for debugging. prints state of machine and first memoryslots
/*void printState(machine *m){
  int i;
  
  printf("Registers:\nr0: %d\tr1: %d\tr2: %d\tr3: %d\tr4: %d\tr5: %d\tsp: %d\tfp: %d\n\n",m->regs[0],m->regs[1],m->regs[2],m->regs[3],m->regs[4],m->regs[5],m->regs[6],m->regs[7]);
  printf("MMU:\nbase: %d\tlimit: %d\tmar: %d\tmbr: %d\n\n",m->mmu->base,m->mmu->limit,m->mmu->mar,m->mmu->mbr);
  printf("CU:\ntr1: %d\ttr2: %d\tir: %d\tpc: %d\tsr: %d\n\n",m->cu->tr1,m->cu->tr2,m->cu->ir,m->cu->pc,m->cu->sr);
  printf("ALU\nin1: %d\tin2: %d\tout: %d\n\n",m->alu->in1,m->alu->in2,m->alu->out);
  for(i=0;i<10;i++)
    printf("%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\n", m->mem[i],m->mem[i+10],m->mem[i+20],m->mem[i+30],m->mem[i+40]);
  printf("\n\n");
}
*/

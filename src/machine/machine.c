#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//project headers
#include <ttk-15.h>
#include "masks.h"
#include "machine.h"
#include "mmu.h"
#include "bitwise.h"

//macros for machine loop
#define getInstruction(M)                \
  mmuGetData(M->mmu, M->mem, M->cu->pc); \
  M->cu->ir = M->mmu->mbr;               \

#define increasePC(M) \
  M->cu->pc++;


static void execInstruction(machine *m, instructionptr *instructions);
static void handleInterrupts(machine *m);

//create new initialized machine
machine *newMachine(long memsize){
  machine       *m   = (machine*)malloc(sizeof(machine));
  alu_unit      *alu = (alu_unit*)malloc(sizeof(alu_unit));
  mm_unit       *mmu = (mm_unit*)malloc(sizeof(mm_unit));
  control_unit  *cu  = (control_unit*)malloc(sizeof(control_unit));

  //initialize all to zero.
  memset(alu, 0, 3* sizeof(MYTYPE));
  memset(mmu, 0, 4* sizeof(MYTYPE));
  memset(cu, 0, 4* sizeof(MYTYPE));

  //set parts together
  m->alu = alu;
  m->mmu = mmu;
  m->cu  = cu;
  
  m->regs = (MYTYPE *) malloc(8* sizeof(MYTYPE));
  m->mem = (MYTYPE *)  malloc(memsize * sizeof(MYTYPE));
  memset(m->mem, 0, memsize);

  //finally set memsize
  m->memsize = memsize;

  return m;
}

//most of execution is done here
void startMachine(machine *m, int debug){
  //create and initialize instruction array
  instructionptr instructions[255];
  initializeInstructionArray(instructions);

  //set limits to mmu
  m->mmu->base = 0;
  m->mmu->limit = m->memsize;
  
  while(1){
    getInstruction(m);

    if(debug){
      printState(m);
      getchar();
    }

    increasePC(m);
    execInstruction(m, instructions);
    //handleInterrupts(m);
  }
}


static void handleInterrupts(machine *m){
  if(m->cu->sr&IFLAG) ;                         //interrupt detected
}


static void execInstruction(machine *m, instructionptr *instructions){
  //prototype for machine instruction
  void(*instruction)(machine*, uint8_t, uint8_t, uint8_t, uint16_t);

  MYTYPE     ins  = m->cu->ir;
  uint8_t    opc  = 0;
  int16_t    addr = 0;
  uint8_t    rj   = 0;
  uint8_t    ri   = 0;
  uint8_t    mode = 0;



  //extract all fields from instruction
  extractOpcode(ins, opc);
  extractAddress(ins, addr);
  extractRj(ins, rj);
  extractRi(ins, ri);
  extractMode(ins, mode);
  

  //value of second operand is stored to temporary register
  m->cu->tr = calculateSecondOperand(m, mode, ri, addr);

  //set ALU operands
  m->alu->in1 = m->regs[rj];
  m->alu->in2 = m->cu->tr;

  instruction = instructions[opc];

  if(instruction == NULL){
    m->cu->sr |= UFLAG;
    fprintf(stderr, "ERROR: incorrect operation code: %X\n", opc);
    exit(-1);
  }

  instruction(m, rj, mode, ri, addr);  
}

//free all memory allocated to machine
void freeMachine(machine *m){
  free(m->regs);
  free(m->alu);
  free(m->mmu);
  free(m->cu);
  free(m->mem);
  free(m);
}

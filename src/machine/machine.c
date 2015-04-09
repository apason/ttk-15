#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//project headers
#include <ttk-15.h>
#include <masks.h>

static void getInstruction(machine *m);
static void increasePC(machine *m);
static void execInstruction(machine *m);
//static void handleInterrupts(machine *m);

machine *newMachine(long memsize){
  machine       *m   = (machine*)malloc(sizeof(machine));
  alu_unit      *alu = (alu_unit*)malloc(sizeof(alu_unit));
  mm_unit       *mmu = (mm_unit*)malloc(sizeof(mm_unit));
  control_unit  *cu  = (control_unit*)malloc(sizeof(control_unit));

  memset(alu, 0, 3* sizeof(MYTYPE));
  memset(mmu, 0, 4* sizeof(MYTYPE));
  memset(cu, 0, 4* sizeof(MYTYPE));
  
  m->alu = alu;
  m->mmu = mmu;
  m->cu  = cu;

  m->regs = (MYTYPE*)malloc(8* sizeof(MYTYPE));
  m->mem = (MYTYPE*)malloc(memsize);
  memset(m->mem, 0, memsize);

  m->memsize = memsize;

  return m;
}

void startMachine(machine *m, int debug){
  m->mmu->base = 0;
  m->mmu->limit = m->memsize;
  while(1){
    getInstruction(m);

    if(debug){
      printState(m);
      getchar();
    }

    increasePC(m);
    execInstruction(m);
    //handleInterrupts(m);
  }
}

static void getInstruction(machine *m){
  mmuGetData(m->mmu,m->mem,m->cu->pc);
  m->cu->ir=m->mmu->mbr;
}

static void increasePC(machine *m){
  m->cu->pc++;
}

/*
static void handleInterrupts(machine *m){
  if(m->cu->sr&IMASK) ;                                   //interrupt detected
}
*/

static void execInstruction(machine *m){
  void(*instruction)(machine*, uint8_t, uint8_t, uint8_t, uint16_t);

  MYTYPE     ins  = m->cu->ir;
  uint8_t    opc  = extractOpcode(ins);
  int16_t    addr = extractAddress(ins);
  uint8_t    rj   = extractRj(ins);
  uint8_t    ri   = extractRi(ins);
  uint8_t    mode = extractMode(ins);

  m->cu->tr = calculateSecondOperand(m, mode, ri, addr); //value of operand

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


//project header
#include <ttk-15.h>

//calculates second operand in instruction (result of addr field and mode field)
MYTYPE calculateSecondOperand(machine *m, uint8_t mode, uint8_t ri, int16_t addr){
  MYTYPE result = 0;
  
  //calculate pointer
  result = calculatePointer(m, mode, ri, addr);

  if(mode != 0){
    mmuGetData(m->mmu, m->mem, result);
    result = m->mmu->mbr;
  }

  return result;
}

/*
 * ri=0 means no index register used 
 * so this function first saves r0, 
 * puts 0 to it and recovers when done
 */

//calculates pointer to second operand in instruction
MYTYPE calculatePointer(machine *m, uint8_t mode ,uint8_t ri ,int16_t addr){
  MYTYPE tmp    = 0;
  MYTYPE result = 0;
  
  tmp = m->regs[0];                      //save r0
  m->regs[0] = 0;                        //set r0 to 0

  result = (MYTYPE) addr;
  result += (m->regs[ri]);               //handle indexed data


  //fetch data from pointer if needed
  if(mode -1 > 0){
    mmuGetData(m->mmu, m->mem, result);
    result = m->mmu->mbr;
  }

  m->regs[0] = tmp;                      //recover r0
  
  return result;  
}

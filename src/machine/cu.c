//project header
#include <ttk-15.h>

//calculates second operand in instruction (result of addr field and mode field)
MYTYPE calculateSecondOperand(machine *m, uint8_t mode, uint8_t ri, int16_t addr){
  //calculate pointer
  addr = calculatePointer(m, mode, ri, addr);

  if(mode != 0){
    mmuGetData(m->mmu, m->mem, addr);
    addr = m->mmu->mbr;
  }

  return addr;
}

//calculates pointer to second operand in instruction

/*
 * ri=0 means no index register used 
 * so this function first saves r0, 
 * puts 0 to it and recovers when done
 */

MYTYPE calculatePointer(machine *m, uint8_t mode ,uint8_t ri ,int16_t addr){
  uint8_t i;
  MYTYPE tmp = m->regs[0];             //save r0
  m->regs[0] = 0;                      //set r0 to 0
  
  addr += (m->regs[ri]);               //handle indexed data

  /* fetch data from pointer if needed (if would
   * be better here cause mode -1 is at most 1  */
  for(i = 0; i < mode - 1; i++){
    mmuGetData(m->mmu, m->mem, addr);
    addr = m->mmu->mbr;
  }

  m->regs[0] = tmp;                    //recover r0
  
  return addr;  
}

//project headers
#include <ttk-15.h>
#include "machine.h"

/*
 * ri=0 means no index register used 
 * so this function first saves r0, 
 * puts 0 to it and recovers when done
 */

//calculates second operand in instruction (result of addr field and mode field)
MYTYPE calculateSecondOperand(machine *m, uint8_t mode, uint8_t ri, int16_t addr){
  int    i      = 0;
  MYTYPE tmp    = m->regs[0];               //save r0
  MYTYPE result = (MYTYPE) addr;

  m->regs[0] = 0;                           //set r0 to 0
  result += m->regs[ri];

  //fetch as many times as needed
  for(i = 0; i < mode; i++){
    mmuGetData(m->mmu, m->mem, result);
    result = m->mmu->mbr;
  }

  m->regs[0] = tmp;                         //restore r0

  return result;
}



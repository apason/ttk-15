#include "ttk-91.h"

MYTYPE calculateSecondOperand(machine *m,uint8_t mode,uint8_t ri,int16_t addr){
  addr=calculatePointer(m,mode,ri,addr);
  if(mode!=0){
    mmuGetData(m->mmu,m->mem,addr);
    addr=m->mmu->mbr;
  }

  return addr;
}

MYTYPE calculatePointer(machine *m, uint8_t mode,uint8_t ri,int16_t addr){
  uint8_t i;
  //ri=0 means no index register used
  MYTYPE tmp=m->regs[0];
  m->regs[0]=0;
  
  addr+=(m->regs[ri]);

  for(i=0;i<mode-1;i++){
    mmuGetData(m->mmu,m->mem,addr);
    addr=m->mmu->mbr;
  }

  m->regs[0]=tmp;
  
  return addr;  
}

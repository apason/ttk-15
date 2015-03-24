#include <stdlib.h>
#include <stdio.h>

//project headers
#include <ttk-91.h>
#include <masks.h>

FUNCTION(nop){
  ;
}

//does this work?
FUNCTION(store){
  mmuSetData(m->mmu, m->mem, m->cu->tr1, m->regs[rj]);
}

FUNCTION(load){
  m->regs[rj] = m->cu->tr2;
}

//simulation
FUNCTION(in){
  int tmp;
  if(m->cu->tr2 == KBD){
    scanf("%d", &tmp);
    m->regs[rj] = tmp;
  }
  else
    error("in instruction IN: reference to unknown device\n");
}

//simulation
FUNCTION(out){
  if(m->cu->tr2 == CRT)
    printf("%d\t", m->regs[rj]);
  else
    error("in instruction OUT: reference to unknown device\n");
}

FUNCTION(add){
  m->alu->out = m->alu->in1 + m->alu->in2;
  m->regs[rj] = m->alu->out;
}

FUNCTION(sub){
  m->alu->out = m->alu->in1 - m->alu->in2;
  m->regs[rj] = m->alu->out;
}

FUNCTION(mul){
  m->alu->out = m->alu->in1 * m->alu->in2;
  m->regs[rj] = m->alu->out;
}

FUNCTION(divv){
  m->alu->out = m->alu->in1 / m->alu->in2;
  m->regs[rj] = m->alu->out;
}

FUNCTION(mod){
  m->alu->out = m->alu->in1 % m->alu->in2;
  m->regs[rj] = m->alu->out;
}

FUNCTION(and){
  m->alu->out = m->alu->in1 & m->alu->in2;
  m->regs[rj] = m->alu->out;
}

FUNCTION(or){
  m->alu->out = m->alu->in1 | m->alu->in2;
  m->regs[rj] = m->alu->out;
}

FUNCTION(xor){
  m->alu->out = m->alu->in1 ^ m->alu->in2;
  m->regs[rj] = m->alu->out;
}

FUNCTION(shl){
  m->alu->out = m->alu->in1 << m->alu->in2;
  m->regs[rj] = m->alu->out;
}

//in progress..
FUNCTION(shr){
  ;
}

FUNCTION(not){
  m->alu->out =~ m->alu->in1;
  m->regs[rj] = m->alu->out;
}

//in progress..
FUNCTION(shra){
  ;
}

FUNCTION(comp){
  //reset GLE bits before comparation
  m->cu->sr &= ~(GMASK|LMASK|EMASK);

  if(m->alu->in1 > m->alu->in2)
    m->cu->sr |= GMASK;
  else if(m->alu->in1 < m->alu->in2)
    m->cu->sr |= LMASK;
  else
    m->cu->sr |= EMASK;
}

/*  ?? should it be possible to jump with ??
 *  ?? indexed of referenced operand      ??
 */

FUNCTION(jump){
  m->cu->pc = mem;
}

FUNCTION(jneg){
  if(m->regs[rj] > 0)
    m->cu->pc = mem;
}

FUNCTION(jzer){
  if(m->regs[rj] == 0)
    m->cu->pc = mem;
}

FUNCTION(jpos){
  if(m->regs[rj] > 0)
    m->cu->pc = mem;
}

FUNCTION(jnneg){
  if(m->regs[rj] >= 0)
    m->cu->pc = mem;
}

FUNCTION(jnzer){
  if(!m->regs[rj])
    m->cu->pc = mem;
}

FUNCTION(jnpos){
  if(m->regs[rj] <= 0)
    m->cu->pc = mem;
}

FUNCTION(jles){
  if(m->cu->sr & LMASK)
    m->cu->pc = mem;
}

FUNCTION(jequ){
  if(m->cu->sr & EMASK)
    m->cu->pc = mem;
}

FUNCTION(jgre){
  if(m->cu->sr & GMASK)
    m->cu->pc = mem;
}

FUNCTION(jnles){
  if(!(m->cu->sr & LMASK))
    m->cu->pc = mem;
}

FUNCTION(jnequ){
  if(!(m->cu->sr & EMASK))
    m->cu->pc = mem;
}

FUNCTION(jngre){
  if(!(m->cu->sr & GMASK))
    m->cu->pc = mem;
}

FUNCTION(call){
  mmuSetData(m->mmu, m->mem, m->regs[rj] +1, m->cu->pc);  //save pc
  mmuSetData(m->mmu, m->mem, m->regs[rj] +2, m->regs[7]); //save fp
  m->regs[rj] += 2;                                       
  m->cu->pc = m->cu->tr1;
  m->regs[7] = m->regs[rj];
}

FUNCTION(exitt){
  mmuGetData(m->mmu, m->mem, m->regs[7] -1);              //get previous pc
  m->cu->pc = m->mmu->mbr;
  mmuGetData(m->mmu, m->mem, m->regs[7]);                 //get previous fp
  m->regs[7] = m->mmu->mbr;

  /* decrease stack pointer.
   * tr1 holds param for exit
   * and +2 comes from pc and fp
   */
  
  m->regs[rj] -= m->cu->tr1 +2;
}

FUNCTION(push){
  m->regs[rj]++;
  mmuSetData(m->mmu, m->mem, m->regs[rj], m->cu->tr2);
}

FUNCTION(pop){
  mmuGetData(m->mmu, m->mem, m->regs[rj]);
  m->regs[ri] = m->mmu->mbr;
  m->regs[rj]--;
}

FUNCTION(pushr){
  int i;
  for(i = 0; i < 7;i++){
    m->regs[rj]++;
    mmuSetData(m->mmu, m->mem, m->regs[rj], m->cu->tr1);
  }
}

FUNCTION(popr){
  int i;
  for(i = 0; i < 7; i++){
    mmuGetData(m->mmu, m->mem, m->regs[rj]);
    m->regs[i] = m->mmu->mbr;
    m->regs[rj]--;
  }
}

//at this point ttk-15 handles all svc commands as halt
FUNCTION(svc){
  exit(0);
}
   

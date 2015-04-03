#include <stdlib.h>
#include <stdio.h>

//project headers
#include <ttk-15.h>
#include <masks.h>

#define MYTYPE_MIN 0x80000000
#define MYTYPE_MAX 0x7FFFFFFF

FUNCTION(nop){
  ;
}

FUNCTION(store){
  mmuSetData(m->mmu, m->mem, m->cu->tr2, m->regs[rj]);
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
  else{
    fprintf(stderr, "ERROR: in instruction IN: reference to unknown device\n");
    freeMachine(m);
    exit(-1);
  }
}

//simulation
FUNCTION(out){
  if(m->cu->tr2 == CRT)
    printf("%d\t", m->regs[rj]);
  else{
    fprintf(stderr, "in instruction OUT: reference to unknown device\n");
    freeMachine(m);
    exit(-1);
  }
}

/*
 *
 * 64bit variables in functions add and sub
 * are for detecting atirhmetical overflows
 *
 */


/*
 * there can not be arithmetic overflows in cases
 * where operands are with different signs so the 
 * checks are only made when operand are similar 
 * to each other
 */

FUNCTION(add){
  int64_t res;
  int64_t in1_64 = m->alu->in1;
  int64_t in2_64 = m->alu->in2;

  if(in1_64 >= 0 && in2_64 >= 0){
    res = in1_64 + in2_64;
    
    if(res > MYTYPE_MAX) m->cu->sr |= OFLAG;      //overflow detected!
  }
  else if(in1_64 < 0 && in2_64 < 0){
    in1_64 = abs(in1_64);
    in2_64 = abs(in2_64);
    res = in1_64 + in2_64;
    
    if(res > MYTYPE_MIN) m->cu->sr |= OFLAG;      //overflow detected!
  }
    
  m->alu->out = m->alu->in1 + m->alu->in2;
  m->regs[rj] = (MYTYPE) m->alu->out;
  
}

/*
 * in this case arithmetical overflow can occur
 * only if the operands are with different sign
 */

FUNCTION(sub){
  int64_t res;
  int64_t in1_64 = m->alu->in1;
  int64_t in2_64 = m->alu->in2;

  if(in1_64 < 0 && in2_64 >= 0){
    in1_64 = abs(in1_64);
    res = in1_64 + in2_64;

    if(res > MYTYPE_MIN) m->cu->sr |= OFLAG;      //overflow detevted!
  }
  else if(in1_64 >= 0 && in2_64 < 0){
    in2_64 = abs(in2_64);
    res = in1_64 + in2_64;

    if(res > MYTYPE_MAX) m->cu->sr |= OFLAG;      //overflow detected!
  }
  
  m->alu->out = m->alu->in1 - m->alu->in2;
  m->regs[rj] = (MYTYPE) m->alu->out;
}

FUNCTION(mul){
  m->alu->out = m->alu->in1 * m->alu->in2;
  m->regs[rj] = (MYTYPE) m->alu->out;

  //check for overflow
  if(m->alu->out / m->alu->in1 != m->alu->in2)
    m->cu->sr |= OFLAG;                           //overflow detected
}

FUNCTION(divv){
  if(m->alu->in2){
    m->alu->out = m->alu->in1 / m->alu->in2;
    m->regs[rj] = (MYTYPE) m->alu->out;
  }
  else
    m->cu->sr |= ZFLAG;                           //division by zero!
}

FUNCTION(mod){
  if(m->alu->in2){
    m->alu->out = m->alu->in1 % m->alu->in2;
    m->regs[rj] = (MYTYPE) m->alu->out;
  }
  else
    m->cu->sr |=ZFLAG;                            //division by zefo!
}

FUNCTION(and){
  m->alu->out = m->alu->in1 & m->alu->in2;
  m->regs[rj] = (MYTYPE) m->alu->out;
}

FUNCTION(or){
  m->alu->out = m->alu->in1 | m->alu->in2;
  m->regs[rj] = (MYTYPE) m->alu->out;
}

FUNCTION(xor){
  m->alu->out = m->alu->in1 ^ m->alu->in2;
  m->regs[rj] = (MYTYPE) m->alu->out;
}

FUNCTION(shl){
  m->alu->out = m->alu->in1 << m->alu->in2;
  m->regs[rj] = (MYTYPE) m->alu->out;
}

FUNCTION(shr){
  MYTYPE mask = 0xFFFFFFFF;

  m->regs[rj] >>= m->cu->tr2;
  mask <<= (mtl -m->cu->tr2);

  m->regs[rj] &= (~mask);
}

FUNCTION(not){
  m->alu->out =~ m->alu->in1;
  m->regs[rj] = (MYTYPE)m->alu->out;
}

FUNCTION(shra){
  int sign;
  MYTYPE mask = 0xFFFFFFFF;

  if(rj < 0) sign = 1;
  else       sign = 0;
    
  m->regs[rj] >>= m->cu->tr2;
  mask <<= (mtl -m->cu->tr2);

  if(sign) m->regs[rj] |= mask;
  else     m->regs[rj] &= (~mask);
}

FUNCTION(comp){
  //reset GLE bits before comparation
  m->cu->sr &= ~(GFLAG|LFLAG|EFLAG);

  if(m->alu->in1 > m->alu->in2)
    m->cu->sr |= GFLAG;
  else if(m->alu->in1 < m->alu->in2)
    m->cu->sr |= LFLAG;
  else
    m->cu->sr |= EFLAG;
}

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
  if(m->cu->sr & LFLAG)
    m->cu->pc = mem;
}

FUNCTION(jequ){
  if(m->cu->sr & EFLAG)
    m->cu->pc = mem;
}

FUNCTION(jgre){
  if(m->cu->sr & GFLAG)
    m->cu->pc = mem;
}

FUNCTION(jnles){
  if(!(m->cu->sr & LFLAG))
    m->cu->pc = mem;
}

FUNCTION(jnequ){
  if(!(m->cu->sr & EFLAG))
    m->cu->pc = mem;
}

FUNCTION(jngre){
  if(!(m->cu->sr & GFLAG))
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
  if(m->cu->tr2 == HALT){
    freeMachine(m);
    exit(0);
  }
  fprintf(stderr, "unknown supervisor call ‰d!\n", m->cu->tr2);
  freeMachine(m);
  exit(-1);
}
   

#include <stdlib.h>
#include <stdio.h>

//project headers
#include <ttk-15.h>
#include "masks.h"
#include "machine.h"
#include "mmu.h"

#define MYTYPE_MIN 0x80000000
#define MYTYPE_MAX 0x7FFFFFFF

static int mtl = sizeof(MYTYPE) * 8;

FUNCTION(nop){
    ;
}

FUNCTION(store){
    mmuSetData(m->mmu, m->mem, m->cu->tr, m->regs[rj]);
}

FUNCTION(load){
    m->regs[rj] = m->cu->tr;
}

//simulation
FUNCTION(in){
    MYTYPE tmp = 0;

    if(m->cu->tr == KBD){
	if(!(m->cu->sr & TFLAG)){
	    while(scanf("%d", &tmp) != 1);
	    m->regs[rj] = tmp;
	}
	else{
	    m->regs[rj] = readInput();
	}
    }

    else{
	fprintf(stderr, "ERROR: in instruction IN: reference to unknown device\n");
	m->cu->sr &= HFLAG;
    }
}

//simulation
FUNCTION(out){
    if(m->cu->tr == CRT){

	if(m->cu->sr & TFLAG)
	    printOutput(m->regs[rj]);
	else
	    printf("%d\t", m->regs[rj]);

    }
    else{
	fprintf(stderr, "in instruction OUT: reference to unknown device\n");
	m->cu->sr |= HFLAG;
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
    int64_t res    = 0;
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

    m->regs[rj] >>= m->cu->tr;
    mask <<= (mtl -m->cu->tr);

    m->regs[rj] &= (~mask);
}

FUNCTION(not){
    m->alu->out =~ m->alu->in1;
    m->regs[rj] = (MYTYPE) m->alu->out;
}

FUNCTION(shra){
    int sign    = -1;
    MYTYPE mask = 0xFFFFFFFF;

    if(rj < 0) sign = 1;
    else       sign = 0;
    
    m->regs[rj] >>= m->cu->tr;
    mask <<= (mtl -m->cu->tr);

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
    m->cu->pc = m->cu->tr;
}

FUNCTION(jneg){
    if(m->regs[rj] > 0)
	m->cu->pc = m->cu->tr;
}

FUNCTION(jzer){
    if(m->regs[rj] == 0)
	m->cu->pc = m->cu->tr;
}

FUNCTION(jpos){
    if(m->regs[rj] > 0)
	m->cu->pc = m->cu->tr;
}

FUNCTION(jnneg){
    if(m->regs[rj] >= 0)
	m->cu->pc = m->cu->tr;
}

FUNCTION(jnzer){
    if(!m->regs[rj])
	m->cu->pc = m->cu->tr;
}

FUNCTION(jnpos){
    if(m->regs[rj] <= 0)
	m->cu->pc = m->cu->tr;
}

FUNCTION(jles){
    if(m->cu->sr & LFLAG)
	m->cu->pc = m->cu->tr;
}

FUNCTION(jequ){
    if(m->cu->sr & EFLAG)
	m->cu->pc = m->cu->tr;
}

FUNCTION(jgre){
    if(m->cu->sr & GFLAG)
	m->cu->pc = m->cu->tr;
}

FUNCTION(jnles){
    if(!(m->cu->sr & LFLAG))
	m->cu->pc = m->cu->tr;
}

FUNCTION(jnequ){
    if(!(m->cu->sr & EFLAG))
	m->cu->pc = m->cu->tr;
}

FUNCTION(jngre){
    if(!(m->cu->sr & GFLAG))
	m->cu->pc = m->cu->tr;
}

FUNCTION(call){
    mmuSetData(m->mmu, m->mem, m->regs[rj] +1, m->cu->pc);  //save pc
    mmuSetData(m->mmu, m->mem, m->regs[rj] +2, m->regs[7]); //save fp
    m->regs[rj] += 2;                                       
    m->cu->pc = m->cu->tr;
    m->regs[7] = m->regs[rj];
}

FUNCTION(exitt){
    mmuGetData(m->mmu, m->mem, m->regs[7] -1);              //get previous pc
    m->cu->pc = m->mmu->mbr;
    mmuGetData(m->mmu, m->mem, m->regs[7]);                 //get previous fp
    m->regs[7] = m->mmu->mbr;

    /* 
     * decrease stack pointer.
     * tr holds param for exit
     * and +2 comes from pc and fp
     */
  
    m->regs[rj] -= m->cu->tr +2;
}

FUNCTION(push){
    m->regs[rj]++;
    mmuSetData(m->mmu, m->mem, m->regs[rj], m->cu->tr);
}

FUNCTION(pop){
    mmuGetData(m->mmu, m->mem, m->regs[rj]);
    m->regs[ri] = m->mmu->mbr;
    m->regs[rj]--;
}

//push registers 0-6 to stack pointed by rj
FUNCTION(pushr){
    int i   = 0;
    int sp  = m->regs[rj];
  
    for(i = 0; i < 7; i++){
	m->regs[rj]++;
	mmuSetData(m->mmu, m->mem, m->regs[rj], *(m->regs +i));
    }
    mmuSetData(m->mmu, m->mem, *(m->regs + rj), sp);
}

//pop registers 0-6 from stack pointed by rj
FUNCTION(popr){
    int i  = 0;
    int sp = 0;
  
    for(i = 6; i >= 0; i--){
	mmuGetData(m->mmu, m->mem, m->regs[rj]);
    
	if(i != rj)
	    m->regs[i] = m->mmu->mbr;
	else
	    sp = m->mmu->mbr;
	m->regs[rj]--;
    }
  
    m->regs[rj] = sp;
}

//could be better
FUNCTION(svc){
    if(m->cu->tr == HALT)
	m->cu->sr |= HFLAG;
    else{
	fprintf(stderr, "unknown supervisor call %d!\n", m->cu->tr);
	freeMachine(m);
	exit(-1);
    }
}

/*
 * floating point extension
 */   

FUNCTION(fin){
    MYTYPEF tmp = 0;
  
    if(m->cu->tr == KBD){
	while(scanf("%f", &tmp) != 1);
	m->regs[rj] = (MYTYPE) tmp;
    }
    else{
	fprintf(stderr, "ERROR: in instruction IN: reference to unknown device\n");
	freeMachine(m);
	exit(-1);
    }
}

FUNCTION(fout){
    if(m->cu->tr == CRT)
	printf("%f\t", (MYTYPEF) m->regs[rj]);
    else{
	fprintf(stderr, "in instruction OUT: reference to unknown device\n");
	freeMachine(m);
	exit(-1);
    }
}

FUNCTION(fadd){
    m->fpu->out = m->fpu->in1 + m->fpu->in2;
    m->regs[rj] = (MYTYPE) m->fpu->out;
}

FUNCTION(fsub){
    m->fpu->out = m->fpu->in1 + m->fpu->in2;
    m->regs[rj] = (MYTYPE) m->fpu->out;
}

FUNCTION(fmul){
    m->fpu->out = m->fpu->in1 * m->fpu->in2;
    m->regs[rj] = (MYTYPE) m->fpu->out;
}

FUNCTION(fdiv){
    m->fpu->out = m->fpu->in1 / m->fpu->in2;
    m->regs[rj] = m->fpu->out;
}

//what to do if out is nan?
FUNCTION(fcomp){
    m->cu->sr &= ~(GFLAG | EFLAG | LFLAG);

    if(m->regs[rj] > m->cu->tr)
	m->cu->sr |= GFLAG;
    else if(m->regs[rj] == m->cu->tr)
	m->cu->sr |= EFLAG;
    else
	m->cu->sr |= LFLAG;
}

FUNCTION(fjneg){
    if(m->regs[rj] < 0)
	m->cu->pc = m->cu->tr;
}

FUNCTION(fjzer){
    if(m->regs[rj] == 0)
	m->cu->pc = m->cu->tr;
}

FUNCTION(fjpos){
    if(m->regs[rj] > 0)
	m->cu->pc = m->cu->tr;
}

FUNCTION(fjnneg){
    if(m->regs[rj] >= 0)
	m->cu->pc = m->cu->tr;
}

FUNCTION(fjnzer){
    if(m->regs[rj] != 0)
	m->cu->pc = m->cu->tr;
}

FUNCTION(fjnpos){
    if(m->regs[rj] <= 0)
	m->cu->pc = m->cu->tr;
}
       

#include <stdio.h>
#include <stdlib.h>

//project header
#include <ttk-15.h>
#include "masks.h"
#include "machine.h"

#define mmuCheckAddress(MMU, X)                                       \
  MMU->mar = X;                                                       \
  if(MMU->mar > MMU->limit || MMU->mar < 0){                          \
    m->cu->sr |= MFLAG;                                               \
    fprintf(stderr, "ERROR: memory reference not permitted %X\n", X); \
    exit(-1);                                                         \
  }                                                                   \


#define mmuSetData(MMU, MEM, ADDR, DATA)   \
  MMU->mbr = DATA;                         \
  mmuCheckAddress(MMU, ADDR);              \
                                           \
  MEM[MMU->mar] = DATA;                    \


#define mmuGetData(MMU, MEM, X)   \
  mmuCheckAddress(MMU, X);        \
  MMU->mbr = MEM[MMU->mar];       \



/* static void mmuCheckAddress(mm_unit *mmu, MYTYPE x); */

/* void mmuGetData(mm_unit *mmu, MYTYPE *mem, MYTYPE x){ */
/*   mmuCheckAddress(mmu, x); */
/*   mmu->mbr = mem[mmu->mar]; */
/* } */

/* void mmuSetData(mm_unit *mmu, MYTYPE *mem, MYTYPE addr, MYTYPE data){ */
/*   mmu->mbr = data; */
/*   mmuCheckAddress(mmu, addr); */

/*   mem[mmu->mar] = data; */
/* } */

/* static void mmuCheckAddress(mm_unit *mmu, MYTYPE x){ */
/*   mmu->mar = x; */
/*   if(mmu->mar > mmu->limit || mmu->mar < 0){ */
/*     //    m->cu->sr |= MFLAG; */
/*     fprintf(stderr, "ERROR: memory reference not permitter: %X\n", x); */
/*     exit(-1); */
/*   }  */
/* } */

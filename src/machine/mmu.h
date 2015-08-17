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
    freeMachine(m);  /* can only be used when m is defined! */	      \
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



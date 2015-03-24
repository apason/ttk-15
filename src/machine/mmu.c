#include <stdio.h>
#include <stdlib.h>

//project header
#include <ttk-91.h>

static void mmuCheckAddress(mm_unit *mmu, MYTYPE x);

void mmuGetData(mm_unit *mmu, MYTYPE *mem, MYTYPE x){
  mmuCheckAddress(mmu, x);
  mmu->mbr = mem[mmu->mar];
}

void mmuSetData(mm_unit *mmu, MYTYPE *mem, MYTYPE addr, MYTYPE data){
  mmu->mbr = data;
  mmuCheckAddress(mmu, addr);

  mem[mmu->mar] = data;
}

static void mmuCheckAddress(mm_unit *mmu, MYTYPE x){
  mmu->mar = x;
  if(mmu->mar > mmu->limit || mmu->mar < 0){
    fprintf(stderr, "ERROR: memory reference not permitter: %X\n", x);
    exit(-1);
  } 
}

#include <stdlib.h>
#include <stdio.h>

//project headers
#include <ttk-15.h>
#include <instructions.h>
#include <ncurses.h>
#include "machine.h"


void initializeInstructionArray(instructionptr *instructions){
  int i = 0;

  //initialize all to null
  for(i = 0; i < 255; i++)
      instructions[i] = haltMachine;

  //initialize valid instructions
  instructions[NOP]   = nop;
  instructions[STORE] = store;
  instructions[LOAD]  = load;
  instructions[IN]    = in;
  instructions[OUT]   = out;
  instructions[ADD]   = add;
  instructions[SUB]   = sub;
  instructions[MUL]   = mul;
  instructions[DIV]   = divv;
  instructions[MOD]   = mod;
  instructions[AND]   = and;
  instructions[OR]    = or;
  instructions[XOR]   = xor;
  instructions[SHL]   = shl;
  instructions[SHR]   = shr;
  instructions[NOT]   = not;
  instructions[SHRA]  = shra;
  instructions[COMP]  = comp;
  instructions[JUMP]  = jump;
  instructions[JNEG]  = jneg;
  instructions[JZER]  = jzer;
  instructions[JPOS]  = jpos;
  instructions[JNNEG] = jnneg;
  instructions[JNZER] = jnzer;
  instructions[JNPOS] = jnpos;
  instructions[JLES]  = jles;
  instructions[JEQU]  = jequ;
  instructions[JGRE]  = jgre;
  instructions[JNLES] = jnles;
  instructions[JNEQU] = jnequ;
  instructions[JNGRE] = jngre;
  instructions[CALL]  = call;
  instructions[EXIT]  = exitt;    //exitt to not to be conflict with std function
  instructions[PUSH]  = push;
  instructions[POP]   = pop;
  instructions[PUSHR] = pushr;
  instructions[POPR]  = popr;
  instructions[SVC]   = svc;

  /*
   * floating point extension
   */

  instructions[FLOAD]  = fload;
  instructions[FIN]    = fin;
  instructions[FOUT]   = fout;
  instructions[FADD]   = fadd;
  instructions[FSUB]   = fsub;
  instructions[FMUL]   = fmul;
  instructions[FDIV]   = fdiv;
  instructions[FCOMP]  = fcomp;
  
  instructions[FJNEG]  = fjneg;
  instructions[FJPOS]  = fjpos;
  instructions[FJZER]  = fjzer;
  instructions[FJNNEG] = fjnneg;
  instructions[FJNZER] = fjnzer;
  instructions[FJNPOS] = fjnpos;

  
}

//mainly for debugging. prints state of machine and first memoryslots
void printState(machine *m){
  int i;
  
  printf("\n\nRegisters:\nr0: %d\tr1: %d\tr2: %d\tr3: %d\tr4: %d\tr5: %d\tsp: %d\tfp: %d\n\n",m->regs[0],m->regs[1],m->regs[2],m->regs[3],m->regs[4],m->regs[5],m->regs[6],m->regs[7]);
  printf("MMU:\nbase: %d\tlimit: %d\tmar: %d\tmbr: %d\n\n",m->mmu->base,m->mmu->limit,m->mmu->mar,m->mmu->mbr);
  printf("CU:\ntr: %d\tir: %d\tpc: %d\tsr: %d\n\n",m->cu->tr,m->cu->ir,m->cu->pc,m->cu->sr);
  printf("ALU\nin1: %d\tin2: %d\tout: %d\n\n",m->alu->in1,m->alu->in2,m->alu->out);
  
  for(i=0;i<10;i++)
    printf("%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\n", m->mem[i],m->mem[i+10],m->mem[i+20],m->mem[i+30],m->mem[i+40]);
  
  printf("\n\n");
}
			      
float f16Decode(f16 f){

    if(f == 0) return 0;
    
    int32_t ret;
    int32_t sign = (f & 0x8000);
    int32_t exp  = (f & 0x7c00) >> 10;
    int32_t man  = (f & 0x3ff );

    ret = ((sign << 16) & 0x80000000) | (((exp -15 +127) << 23) & 0xff800000) | ((man << 13) & 0xffffe000);

    return *(volatile float*) &ret;
}

header_data *readHeader(FILE *fp){
    MYTYPE header_end, i;
    position_list *pl = NULL;
    header_data *header = (header_data *)malloc(sizeof(header_data));

    fread(&header_end, sizeof(MYTYPE), 1, fp); //check return value for error!

    //if there is no additional debug data ( -g not used)
    if((header_end / sizeof(MYTYPE) -1) / 2 <= 0){ //if header_end == 4
	header->pl = NULL;
	header->usage_start = -1;
	return header;
    }

    fseek(fp, 0, SEEK_SET);

    pl = (position_list *)malloc(sizeof(position_list));
    header->pl = pl;
    
    //read that many pairs of (code_start, data_start)
    
    for(i = 0; i < (header_end / sizeof(MYTYPE) -3) / 2; i++){
	
	fread(&pl->data, sizeof(MYTYPE), 1, fp);
	fread(&pl->code, sizeof(MYTYPE), 1, fp);

	pl->next = (position_list *)malloc(sizeof(position_list));
	pl = pl->next;
    }

    pl->next = NULL;
    
    fread(&pl->data, sizeof(MYTYPE), 1, fp);
    fread(&pl->code, sizeof(MYTYPE), 1, fp);

    fread(&header->usage_start, sizeof(MYTYPE), 1, fp);

    return header;
}

usage_list *readUsages(FILE *fp, int usage_start){
    int i, d, tmp;
    usage_list *ul = NULL;
    
    if(usage_start > 0)
	return NULL;

    fseek(fp, 0, SEEK_END);
    tmp = ftell(fp);
    fseek(fp, usage_start, SEEK_SET);
    d = ftell(fp);
    d = tmp -d;

    if(d % (LABELLENGTH + sizeof(MYTYPE)) != 0){
	printf("ERRORRR. incorrect usage table");
	fflush(NULL);
    }

    ul = (usage_list *)malloc(sizeof(usage_list));
    ul->next = NULL;
    
    for(i = 0; i < d; i++){
	fread(&ul->value, sizeof(MYTYPE), 1, fp);
	fread(&ul->label, LABELLENGTH, 1, fp);

	if(i < d -1){
	    ul->next = (usage_list *)malloc(sizeof(usage_list));
	    ul = ul->next;
	    ul->next = NULL;
	}
    }
    
    return ul;
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//project headers
#include <ttk-15.h>
#include <instructions.h>
#include <ncurses.h>
#include <disasm.h>
#include "machine.h"
#include "bitwise.h"

//max length of disassembled instruction
#define MAXLEN 150

static void disAssemble(const MYTYPE *source, char *code_table, usage_list *ul, int i, MYTYPE *mem, int max_label_length);
static char *getLabelPrefix(int16_t line, usage_list *ul, int addr, MYTYPE *mem);
static char *getLabelSuffix(int16_t line, usage_list *ul);
static void adjustPositionLists(position_list *pl);
static int isCodeArea(int i, position_list *pl);
int maxLabelLength(usage_list *ul);

static char *reg_table[] = {"r0,", "r1,", "r2,", "r3,", "r4,", "r5,", "sp,", "fp,"};
static char *mod_table[] = {" =", " ", " @", " ?"};
static char *ind_table[] = {" ", "(r1)", "(r2)", "(r3)", "(r4)", "(r5)", "(sp)", "(fp)"};


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

void initializeDisAssemblyArray(void){
    
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

    //Error. too small header. (less then one word)
    if(fread(&header_end, sizeof(MYTYPE), 1, fp) != 1){
	printf("1");
	free(header);
	return NULL;
    }

    //Error. incorrect binary file
    //NOTE. header_end can be negative value!
    if((header_end >= 0 ? header_end : -header_end) % sizeof(MYTYPE) != 0){
	printf("end: %d\t", header_end);
	free(header);
	return NULL;
    }

    //if there is no additional debug data ( -g not used)
    if(header_end < 0){
	header->pl = NULL;
	header->usage_start = -1;
	return header;
    }

    //-g used. check header size. header_end can not be a negative number
    fseek(fp, 0, SEEK_END);
    if(ftell(fp) < header_end){
	printf("3");
	free(header);
	return NULL;
    }
    
    //start from beginning
    fseek(fp, 0, SEEK_SET);

    pl = (position_list *)malloc(sizeof(position_list));
    header->pl = pl;
    
    //read n -1 pairs of (code_start, data_start)
    for(i = 0; i < (header_end / sizeof(MYTYPE) -3) / 2; i++){
	
	fread(&pl->code, sizeof(MYTYPE), 1, fp);
	fread(&pl->data, sizeof(MYTYPE), 1, fp);

	pl->code /= sizeof(MYTYPE);
	pl->data /= sizeof(MYTYPE);

	pl->next = (position_list *)malloc(sizeof(position_list));
	pl = pl->next;
    }

    pl->next = NULL;

    //read the last modules code and data
    fread(&pl->code, sizeof(MYTYPE), 1, fp);
    fread(&pl->data, sizeof(MYTYPE), 1, fp);

    pl->code /= sizeof(MYTYPE);
    pl->data /= sizeof(MYTYPE);

    fread(&header->usage_start, sizeof(MYTYPE), 1, fp);

    return header;
}

usage_list *readUsages(FILE *fp, int usage_start){
    int i, d, tmp;
    usage_list *ul = NULL;
    usage_list *head = NULL;
    
    if(usage_start < 0)
	return NULL;

    /*
     * calculate the size of usage_list 
     * and set stream position to the beginning
     */
    fseek(fp, 0, SEEK_END);
    tmp = ftell(fp);
    d = (tmp -usage_start);                          // size in BYTES
    fseek(fp, usage_start, SEEK_SET);

    //error checking
    if(d % (LABELLENGTH + sizeof(MYTYPE)) != 0){
	printf("ERRORRR. incorrect usage table");
	fflush(NULL);
    }
    else
	/*
	 * size in 36? byte FIELDS
	 * one field contains row number and the label
	 */
	d /= (LABELLENGTH + sizeof(MYTYPE));         // size in 

    //reserve memory for the first unit
    ul = (usage_list *)malloc(sizeof(usage_list));
    ul->next = NULL;
    head = ul;                                       // save list start

    //read all label usages
    for(i = 0; i < d; i++){
	fread(&ul->value, sizeof(MYTYPE), 1, fp);
	fread(&ul->label, LABELLENGTH, 1, fp);

	//reserve memory for next field.
	if(i < d -1){
	    ul->next = (usage_list *)malloc(sizeof(usage_list));
	    ul = ul->next;
	    ul->next = NULL;
	}
    }
    
    return head;
}


/*
 * Retutns the value length of all code + data(excluding last module)
 * start of usage list - end of header
 * in WORDS
 */
int codeLength(header_data *header, FILE *fp){
    int i, last_data, ret;
    position_list *pl;

    /*
     * if modules were linked without debug flag
     * there is counter value of the last code line
     * address in the first word.
     */
    if(header->pl == NULL){
	fseek(fp, 0, SEEK_SET);
	fread(&ret, sizeof(MYTYPE), 1, fp);
	return -ret;
    }

    for(pl = header->pl, i = 0; pl; pl = pl->next){
	i += 2;
	if(pl->next == NULL)
	    last_data = header->usage_start / sizeof(MYTYPE) -pl->data;
    }

    //there is 2*n +1 fields in the header
    return (header->usage_start / sizeof(MYTYPE)) -(i +1) -last_data;
    
}

static void adjustPositionLists(position_list *pl){

    //not linked with -g
    if(!pl) return;
    
    int address_constant = pl->code;

    for(; pl; pl = pl->next){
	pl->code -= address_constant;
	pl->data -= address_constant;
    }
}

char **constructCodes(position_list *pl, usage_list *ul, int length, MYTYPE *mem){
    int i, max_label_length;
    char **code_table = (char **)malloc(length * sizeof(char*));
        printf("asd"); fflush(NULL);
    adjustPositionLists(pl);
        printf("asd"); fflush(NULL);
    printPositionList(pl); fflush(NULL);
    printf("asd"); fflush(NULL);
    max_label_length = maxLabelLength(ul);
    printf("asd"); fflush(NULL);
    for(i = 0; i <= length; i++)
	if(isCodeArea(i, pl)){
	    code_table[i] = (char *)malloc(MAXLEN * sizeof(char));
	    disAssemble(mem, code_table[i], ul, i, mem, max_label_length +1);
	}
	else
	    code_table[i] = NULL;

    return code_table;
}

static int isCodeArea(int i, position_list *pl){

    //if pl is null we dont know about code and data areas so disassemble all of them
    if(pl == NULL)
	return 1;

    for(; pl; pl = pl->next)
	if(i >= pl->code && i < pl->data)
	    return 1;

    return 0;
}

static void disAssemble(const MYTYPE *source, char *code_table, usage_list *ul, int i, MYTYPE *mem, int max_label_length){
    char     *label = NULL;

    uint8_t   opc   = 0;
    int16_t   addr  = 0;
    uint8_t   rj    = 0;
    uint8_t   ri    = 0;
    uint8_t   mode  = 0;

    //extract all fields from instruction (macros)
    extractOpcode(source[i], opc);
    extractAddress(source[i], addr);
    extractRj(source[i], rj);
    extractRi(source[i], ri);
    extractMode(source[i], mode);

    label = getLabelPrefix(i, ul, addr, mem);
    if(label)
	sprintf(code_table, "%-*s ", max_label_length, label);
    else
	sprintf(code_table, "%-*d ", max_label_length, i);
    printf("%d\n", opc); fflush(NULL);
    opc = opc <= 166 ? opc : 10; // set to unknown
    printf("%d", opc); fflush(NULL);
    sprintf(code_table +max_label_length, "%-7s", dis_asm[opc] != NULL ? dis_asm[opc] : " ");

    //ERROR. same problem here!!!
    strncat(code_table, reg_table[rj], 3);
    
    strncat(code_table, mod_table[mode], 2);

    label = getLabelSuffix(i, ul);

    if(label)
	strncat(code_table, label, 33);
    else
	sprintf(code_table +strlen(code_table), "%d", addr);

    strncat(code_table, ind_table[ri], 5);


}




static char *getLabelSuffix(int16_t line, usage_list *ul){
    for(; ul; ul = ul->next)
	if(ul->value == line)
	    return ul->label;

    return NULL;
}

//search ALL labels and check if addr is  line
static char *getLabelPrefix(int16_t line, usage_list *ul, int addr, MYTYPE *mem){
    MYTYPE tmp;

    for(; ul; ul = ul->next){
	extractAddress(mem[ul->value], tmp);	
	if(tmp == line)
	    return ul->label;
    }
	
    return NULL;
}

void printHeader(header_data *header){
    if(header == NULL){
	printf("NULL\n");
	return;
    }
    if(header->pl == NULL)
	printf("position_list:\tNULL\n");
    else
	printPositionList(header->pl);

    printf("usage_start:\t%d\n", header->usage_start);
}

void printPositionList(position_list *pl){
    int i = 0;
    for(; pl; pl = pl->next)
	printf("module %d:\tcode: %d\tdata: %d\n", i++, pl->code, pl->data);
}
    
void printUsageList(usage_list *ul){
    int i = 0;
    for(; ul; ul=ul->next)
	printf("%d\t%d: %s\n", i++, ul->value, ul->label);
}

int maxLabelLength(usage_list *ul){
    int max = 0;

    for(; ul; ul = ul->next)
	max = strlen(ul->label) > max ? strlen(ul->label) : max;

    return max;
}

/*
 * this function checks that values in header are in possible order.
 * that means data block of a module is allways after the corresponding
 * code block. Also values from first module should be before the values
 * of the second module etc. Usage start should locate after all data
 * and code blocks.
 *
 * Size of the binary should be bigger or equal to usage start
 *
 * returns 1 if above conditions are true, 0 otherwise.
 *
 */
int checkHeaderIntegrityAndFileSize(header_data *header, FILE *fp){
    MYTYPE last_data = 0;
    position_list *pl = header->pl;
    size_t tmp;

    //linked without -g
    if(header->usage_start == -1)
	return 1;
    
    for(; pl; pl = pl->next){
	if(pl->code < 0 || pl->data < 0 || \
	   pl->code > pl->data || pl->code < last_data)
	    return 0;

	last_data = pl->data;
    }

    tmp = ftell(fp);
    fseek(fp, 0, SEEK_END);

    //this is ok because the binari is linked with -g. (usage start > 0)
    if(ftell(fp) < header->usage_start)
	return 0;

    fseek(fp, tmp, SEEK_SET);

    return 1;
}

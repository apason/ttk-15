#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <ncurses.h>
#include <ttk-15.h>
#include "machine.h"
#include "masks.h"

struct outputList{
    MYTYPE output;
    type_param tpar;
    struct outputList *next;
};

struct disassembled_data{
    char **codes;
    int offset;
    int max_value;
    int selected;
};

/* 
 * describes how many characters is needed to present
 * the value of MYTYPE(F) in specific representation
 */
enum type { BIN = 32, HEX = 8, DEC = 11, EXP = 13 };
//current screen tab to draw
enum screentab { CPU, MEM, OUT };

static int isCodeArea(int line);
static void drawSelected(const machine *m);
static WINDOW *drawMYTYPE(MYTYPE *addr, int elems, int x, char **arr, int size);
static WINDOW *drawMYTYPEF(MYTYPEF *addr, int elems, enum type current, int x, char **arr, int size);
static int calculateRow(int x, enum type current, int a, int size);
static int len(MYTYPE x);
static void drawCPU(const machine *m);
static void drawMemory(const mm_unit *mmu, const MYTYPE *mem);
static void drawCRT(void);
static void drawPanel(void);
static void nextMemoryPage(MYTYPE limit);
static void prevMemoryPage(MYTYPE limit);
static void nextMemoryLine(MYTYPE limit);
static void prevMemoryLine(MYTYPE limit);
static void endMemory(MYTYPE limit);
static void homeMemory(void);
static void nextCRTPage(void);
static void prevCRTPage(void);
static void nextCRTLine(void);
static void prevCRTLine(void);
static void homeCRT(void);
static void endCRT(void);
static void printBin(WINDOW *w, MYTYPE a);
static int listLength(const struct outputList *l);
static int notCorrectInput(const char *buffer);
static WINDOW *drawDAB(MYTYPE *memory, int x, int y);
static void prevDABLine(int *bind);
static void nextDABLine(int *bind);

static char ra0[] = "R0:";
static char ra1[] = "R1:";
static char ra2[] = "R2:";
static char ra3[] = "R3:";
static char ra4[] = "R4:";
static char ra5[] = "R5:";
static char ra6[] = "SP:";
static char ra7[] = "FP:";
static char *registers_array[]={ra0, ra1, ra2, ra3, ra4, ra5, ra6, ra7};

static char ma0[] = "limit:";
static char ma1[] = "base:";
static char ma2[] = "MBR:";
static char ma3[] = "MAR:";
static char *MMU_array[] = {ma0, ma1, ma2, ma3};

static char ca0[] = "TR:";
static char ca1[] = "IR:";
static char ca2[] = "PC:";
static char ca3[] = "SR:";
static char *CU_array[] = {ca0, ca1, ca2, ca3};

static char aa0[] = "in1:";
static char aa1[] = "in2:";
static char aa2[] = "out:";
static char *ALU_array[] = {aa0, aa1, aa2};

static char fa0[] = "in1:";
static char fa1[] = "in2:";
static char fa2[] = "out:";
static char *FPU_array[] = {fa0, fa1, fa2};

static struct disassembled_data disassembled;
static char *breakpoints;

static enum type current;
static struct outputList *first = NULL;
static enum screentab scr;

//data from initialization
static int current_memory_row;
static int crt_notification;
static int crt_offset;
static position_list *pl;
static const MYTYPE *pc;
    
/*
 * main function of ncurses gui. this is executed
 * between every instruction in debugging mode
 */
void drawScreen(machine *m){
    int ch;
    static int start = 1;
    static int to_end = 0;
    static int bind = 1;

    if(to_end)
	return;
    
    //do not draw the screen!
    if(!breakpoints[m->cu->pc]){
	if(start == 0)
	    return;
	else
	    start = 0;
    }

    //adjust selection line to pc
    if(bind){
	for(; disassembled.selected < *pc; prevDABLine(&bind));
	for(; disassembled.selected > *pc; nextDABLine(&bind));
    }
    

    drawSelected(m);
    
    //return only if we are in CPU window and enter is pressed
    while((ch = getch()) != 10){
	switch (ch){
	    //tab selection
	case KEY_F(1):
	    scr = CPU;	    
	    break;
	case KEY_F(2):
	    scr = MEM;	    
	    break;
	case KEY_F(3):
	    scr = OUT;	    
	    break;
	    //output type selection
	case 'x':
	    current = HEX;
	    break;
	case 'b':
	    current = BIN;
	    break;
	case 'd':
	    current = DEC;
	    break;
	    //navigation
	case KEY_NPAGE:
	    if(scr == MEM)
		nextMemoryPage(m->mmu->limit);
	    if(scr == OUT)
		nextCRTPage();
	    break;
	case KEY_PPAGE:
	    if(scr == MEM)
		prevMemoryPage(m->mmu->limit);
	    if(scr == OUT)
		prevCRTPage();
	    break;
	case KEY_DOWN:
	    if(scr == MEM)
		nextMemoryLine(m->mmu->limit);
	    if(scr == OUT)
		nextCRTLine();
	    if(scr == CRT)
		prevDABLine(&bind);
	    break;
	case KEY_UP:
	    if(scr == MEM)
		prevMemoryLine(m->mmu->limit);
	    if(scr == OUT)
		prevCRTLine();
	    if(scr == CRT)
		nextDABLine(&bind);
	    break;
	case KEY_HOME:
	    if(scr == MEM)
		homeMemory();
	    if(scr == OUT)
		homeCRT();
	    break;
	case KEY_END:
	    if(scr == MEM)
		endMemory(m->mmu->limit);
	    if(scr == OUT)
		endCRT();
	    break;
	    //breakpoint commands
	case 'n':
	    start = 0;
	    return;
	    break;
	case 'e':
	    m->cu->sr = ~TFLAG;
	    to_end = 1;
	    return;
	    break;
	case ' ':
	    if(scr == CPU)
		breakpoints[disassembled.selected] ^= 1;
	    break;
	default:
	    ;
	}
	drawSelected(m);
    }
    
    start = 1;
}

static void endCRT(void){
    crt_offset = 0;
}

static void homeCRT(void){
    int x, y;
    (void) x;
    (void) y;
    getmaxyx(stdscr, y, x);
    crt_offset = listLength(first) -y +4;
}

static void prevCRTLine(void){
    int x, y;
    (void) x;
    getmaxyx(stdscr, y, x);
    crt_offset++;
    if(crt_offset > listLength(first) -y +4)
	crt_offset = listLength(first) -y +4;
}

static void nextCRTLine(void){
    crt_offset--;
    if(crt_offset < 0)
	crt_offset = 0;
}

static void nextDABLine(int *bind){
    disassembled.selected--;
    if(disassembled.selected < 0)
	disassembled.selected = 0;
    
    //data addresses cannot be selected!
    for(; isCodeArea(disassembled.selected) == 0; disassembled.selected--);
    
    if(disassembled.selected == *pc)
	*bind = 1;
    else
	*bind = 0;
}

static void prevDABLine(int *bind){
    disassembled.selected++;
    if(disassembled.selected > disassembled.max_value)
	disassembled.selected = disassembled.max_value;

    //data address cannot be selected!
    for(; isCodeArea(disassembled.selected) == 0; disassembled.selected++);

    if(disassembled.selected == *pc)
	*bind = 1;
    else
	*bind = 0;
}

static int listLength(const struct outputList *l){
    int i;
    for(i = 0; l; l = l->next)
	i++;
    return i;
}

static void prevCRTPage(void){
    int x, y;
    (void) x;
    getmaxyx(stdscr, y, x);
    crt_offset += y -4;
    if(crt_offset > listLength(first) -y +4)
	crt_offset = listLength(first) -y +4;
}

static void nextCRTPage(void){
    int x, y;
    (void) x;
    getmaxyx(stdscr, y, x);
    crt_offset -= y -4;
    if(crt_offset < 0)
	crt_offset = 0;
}

static void homeMemory(void){
    current_memory_row = 0;
}

static void endMemory(MYTYPE limit){
    int x, y, slots_per_row;

    getmaxyx(stdscr, y, x);
    slots_per_row = (x -2) / (current +len(limit) +5);

    current_memory_row = limit -(slots_per_row * (y -5));
}

static void nextMemoryLine(MYTYPE limit){
    int x, y, slots_per_row;

    getmaxyx(stdscr, y, x);
    slots_per_row = (x -2) / (current +len(limit) +5);

    current_memory_row += slots_per_row;;

    if(current_memory_row >= limit -(slots_per_row * (y -5)))
	current_memory_row = limit -(slots_per_row * (y -5));
}

static void prevMemoryLine(MYTYPE limit){
    int x, y, slots_per_row;

    (void) y;

    getmaxyx(stdscr, y, x);
    slots_per_row = (x -2) / (current +len(limit) +5);

    current_memory_row -= slots_per_row;

    if(current_memory_row < 0) current_memory_row = 0;
}

static void nextMemoryPage(MYTYPE limit){
    int x, y, slots_per_row;

    getmaxyx(stdscr, y, x);
    slots_per_row = (x -2) / (current +len(limit) +5);

    current_memory_row += slots_per_row * (y -5);

    if(current_memory_row >= limit -(slots_per_row * (y -5)))
	current_memory_row = limit -(slots_per_row * (y -5));
}

static void prevMemoryPage(MYTYPE limit){
    int x, y, slots_per_row;

    getmaxyx(stdscr, y, x);
    slots_per_row = (x -2) / (current +len(limit) +5);

    current_memory_row -= slots_per_row * (y -5);

    if(current_memory_row < 0) current_memory_row = 0;
}

static void drawSelected(const machine *m){
    switch (scr){
    case CPU:
	drawCPU(m);
	break;
    case MEM:
	drawMemory(m->mmu, m->mem);
	break;
    case OUT:
	drawCRT();
	break;
    }
}

static void drawMemory(const mm_unit *mmu, const MYTYPE *mem){
    int x, y, rows, cols, slots_per_row, i, j, max_slot, k, l;

    (void) l;

    wbkgd(stdscr, COLOR_PAIR(1));

    werase(stdscr);
    
    box(stdscr, 0, 0);

    drawPanel();

    getmaxyx(stdscr, y, x);


    (void) cols;
    rows = y -4; //borders guidline and empty row
    cols = (x -2) / (current +len(mmu->limit) +5);
    slots_per_row = (x -2) / (current +len(mmu->limit) +5);

    max_slot = slots_per_row * rows < mmu->limit ? slots_per_row * rows : mmu->limit;
    

    wmove(stdscr, 2, 1);
    for(i = current_memory_row; i < (slots_per_row * rows) +current_memory_row && i < mmu->limit; i++){
	if(i % slots_per_row == 0){
	    getyx(stdscr, y, x);
	    wmove(stdscr, y +1, 1);
	}

	getyx(stdscr, l, k);

	if(i == 0)
	    for(j = 0; j < len(max_slot) -1; j++)
		wprintw(stdscr, " ");
	else
	    for(j = i; len(j) < len(max_slot); j *= 10)
		wprintw(stdscr, " ");

	if(current == DEC)
	    wprintw(stdscr, " %d: %d  ", i, mem[i]);
	if(current == HEX)
	    wprintw(stdscr, " %d: %x  ", i, mem[i]);
	if(current == BIN){
	    wprintw(stdscr, " %d: ", i);
	    printBin(stdscr, mem[i]);
	    wprintw(stdscr, "  ");
	}
	
	getyx(stdscr, y, x);
	wmove(stdscr, y, x -(x -k) +current +len(max_slot) +5);
    }
    refresh();
}

static void drawCRT(void){
    int x, y, max_y, i, items = 0;
    struct outputList *tmp;

    crt_notification = 0;
    
    werase(stdscr);
    box(stdscr, 0, 0);

    drawPanel();
    
    (void) x;

    getmaxyx(stdscr, y, x);

    max_y = y -4;

    for(tmp = first; tmp; tmp = tmp->next) items++;
    
    tmp = first;
    for(i = 0; i < items -max_y -crt_offset; i++) tmp = tmp->next;
    
    wmove(stdscr, 3, 1);

    for(i = 0; tmp && i < max_y; tmp = tmp->next){
	if(current == DEC){
	    if(tmp->tpar == MYTYPE_PARAM)
		wprintw(stdscr, " %d ", tmp->output);
	    else if(tmp->tpar == MYTYPEF_PARAM)
		wprintw(stdscr, " %g ", *(volatile MYTYPEF*) &tmp->output);
	}
	else if(current == HEX)
	    wprintw(stdscr, " %x ", tmp->output);
	else if(current == BIN){
	    wprintw(stdscr, " ");
	    printBin(stdscr, tmp->output);
	}
	getyx(stdscr, y, x);
	wmove(stdscr, y +1, 1);
	i++;
    }
    refresh();
}

static void drawCPU(const machine *m){
    int y, x, pos = 3;
    static WINDOW *registers, *MMU, *CU, *ALU, *FPU, *DAB;

    wbkgd(stdscr, COLOR_PAIR(1));

    //clear all output from last draw
    werase(stdscr);
    delwin(registers);
    delwin(MMU);
    delwin(CU);
    delwin(ALU);
    delwin(FPU);
    delwin(DAB);
    
    //macro
    getmaxyx(stdscr, y, x);

    //create subwindows for every part of cpu
    registers = drawMYTYPE(m->regs, 8, x -2, (char**)registers_array, 3);
    MMU = drawMYTYPE(&m->mmu->limit, 4, x -2, (char**)MMU_array, 6);
    CU = drawMYTYPE(&m->cu->tr, 4, x -2, (char**)CU_array, 3);
    ALU = drawMYTYPE(&m->alu->in1, 3, x -2, (char**)ALU_array, 4);
    FPU = drawMYTYPEF(&m->fpu->in1, 3, current, x -2, (char**)FPU_array, 4);
    
    //title them
    wprintw(registers, "registers");
    wprintw(MMU, "MMU");
    wprintw(CU, "CU");
    wprintw(ALU, "ALU");
    wprintw(FPU, "FPU");

    //calculate positions of subwindows
    mvwin(registers, pos, 1);
    getmaxyx(registers, y, x);
    pos += y;
    mvwin(MMU, pos, 1);
    getmaxyx(MMU, y, x);
    pos += y;
    mvwin(CU, pos, 1);
    getmaxyx(CU, y, x);
    pos += y;
    mvwin(ALU, pos, 1);
    getmaxyx(ALU, y, x);
    pos += y;
    mvwin(FPU, pos, 1);
    getmaxyx(FPU, y, x);
    pos += y;
    
    getmaxyx(stdscr, y, x);
    
    //print DisAssembler and Breakpoints
    DAB = drawDAB(m->mem, x -2, y -pos -1);
    mvwin(DAB, pos, 1);

    box(stdscr, 0, 0);
    drawPanel();
	    
    //print subwindows and main window
    refresh();    
    wrefresh(registers);
    wrefresh(MMU);
    wrefresh(CU);
    wrefresh(ALU);
    wrefresh(FPU);
    wrefresh(DAB);
    
}

void killScreen(void){
    endwin();
}

/*
 * drawMYTYPE and drawMYTYPEF functions creates and
 * draws specific windows for cpu parts. first parameter
 * is pointer to first element to draw and current
 * describes how many characters is needed to present
 * the register (includes sign and space after the element.
 * NOTE! when passing pointer to struct it is not guaranteed
 * by C standard that addr +1 is the second object of that struct
 * so this is not necessarily work! bad code :-(
 */
static WINDOW *drawMYTYPE(MYTYPE *addr, int elems, int x, char *arr[], int size){
    WINDOW *w;
    int row, regsperrow, i, j, currentx, currenty, count, m, n;

    (void) n;

    //how many rows we need to dras all elements
    // -2 for window borded
    row = calculateRow(x -2, current +size +2 , elems, size);
    //how many elements could be drawn to one line
    regsperrow = (x -2) / (current +size +2);
    
    w = newwin(row +2, x, 0, 0);              // +2 for window borded
    
    wbkgd(w, COLOR_PAIR(1));

    box(w, 0, 0);

    wmove(w, 1, 1);
    count = 0;
    
    for(i = 0; i < row; i++){
	for(j = 0; j < regsperrow && count < elems; j++){

	    getyx(w, n, m);

	    //print one item
	    if(current == DEC)
		wprintw(w, "%s %d ", arr[count], addr[count]);
	    else if(current == HEX)
		wprintw(w, "%s %x ", arr[count], addr[count]);
	    else if(current == BIN){
		wprintw(w, "%s ", arr[count]);
		printBin(w, addr[count]);
		wprintw(w, " ");
	    }

	    count++;

	    //move cursor to start of second item
	    getyx(w, currenty, currentx);
	    wmove(w, currenty, currentx -(currentx -m) +current +size +2);
	}
	//move cursor to beginning of second line
	getyx(w, currenty, currentx);
	wmove(w, currenty +1, 1);
    }
    //is this needed?
    wmove(w, 0, 0);

    return w;
}
//what if disassembled.offset happens to be in data area?
static WINDOW *drawDAB(MYTYPE *memory, int x, int y){
    WINDOW *w;
    int i, j, current_y, current_x, in_data = 0;
    static char *brprefix[2] = {"[ ] ", "[x] "};
    
    (void) current_x;

    w = newwin(y, x, 0, 0);

    /*
     * adjust offset to corrsepond selection. 
     * selection should allways been visible.
     */
    while(disassembled.selected < disassembled.offset){
	disassembled.offset -=y;
	if(disassembled.offset < 0)
	    disassembled.offset = 0;
	for(; isCodeArea(disassembled.offset) == 0; disassembled.offset--);
    }
    while(disassembled.selected > disassembled.offset +y){
	disassembled.offset +=y;
	if(disassembled.offset > disassembled.max_value -y)
	    disassembled.offset = disassembled.max_value -y;
	for(; isCodeArea(disassembled.offset) == 0; disassembled.offset++);
    }    


    for(i = disassembled.offset, j = 0; j < y && i <= disassembled.max_value; j++, i++){
	if(isCodeArea(i)){
	    //set color
	    if(i == *pc)
		wattron(w, COLOR_PAIR(3));
	    else if(i == disassembled.selected)
		wattron(w, COLOR_PAIR(4));
	    else
		wattroff(w, COLOR_PAIR(1));
	    
	    wprintw(w, "%s %s", brprefix[(int) breakpoints[i]], disassembled.codes[i]);
	    getyx(w, current_y, current_x);
	    wmove(w, current_y +1, 0);
	    in_data = 0;
	}
	else{
	    if(in_data == 0){
		wprintw(w, " ");
		getyx(w, current_y, current_x);
		wmove(w, current_y +1, 0);
	    }
	    else
		j--;
	    in_data++;
	}
    }
    return w;
}
	
//if current was global this function would fuck it up.
static WINDOW *drawMYTYPEF(MYTYPEF *addr, int elems, enum type current, int x, char *arr[], int size){
    WINDOW *w;
    int row, regsperrow, i, j, currentx, currenty, count, m, n;

    (void) n;


    if(current == DEC) current = EXP;

    row = calculateRow(x -2, current +size +2, elems, size);
    regsperrow = (x -2) / (current +size +2);
    
    w = newwin(row +2, x, 0, 0);

    wbkgd(w, COLOR_PAIR(1));

    box(w, 0, 0);

    wmove(w, 1, 1);
    count = 0;
    for(i = 0; i < row; i++){
	for(j = 0; j < regsperrow && count < elems; j++){

	    getyx(w, n, m);

	    if(current == EXP)
		wprintw(w, "%s %g", arr[count], *(addr +count));
	    else if(current == HEX)  //will not work on real floats!
		wprintw(w, "%s %x ", arr[count], *((MYTYPE*)(addr + count)));
	    else if(current == BIN){
		wprintw(w, "%s ", arr[count]);
		printBin(w, *(MYTYPE*)&addr[count]);
		wprintw(w, " ");
	    }

	    count++;

	    getyx(w, currenty, currentx);
	    
	    wmove(w, currenty, currentx -(currentx -m) +current +size +2);
	}
	getyx(w, currenty, currentx);
	wmove(w, currenty +1, 1);
    }

    wmove(w, 0, 0);

    return w;
}

//paska funktio! ei toimi ku desimaaleil! x(
static int len(MYTYPE x){
    int len = 1;

    //possible owerflow! works only with decimals!!!
    if(x < 0){
	x = x * (-1);
	len++;
    }

    for(; x >= 10; x /= 10)
	len++;

    return len;
}

void initScreen(char **disassembled_codes, int length, position_list *poslist, const MYTYPE *program_counter){
    int i;

    pl = poslist;

    pc = program_counter;

    disassembled.codes  = disassembled_codes;
    disassembled.max_value  = length -1;
    disassembled.offset = 0;
    disassembled.selected = 0;

    //no breakpoints after initialization
    breakpoints = (char *)malloc(length * sizeof(char));
    for(i = 0; i < length; i++)
	breakpoints[i] = 0;
    
    scr = CPU;
    initscr();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);  //default hilight
    init_pair(2, COLOR_BLUE, COLOR_WHITE);  //default color
    init_pair(3, COLOR_BLACK, COLOR_RED);   //default pc
    init_pair(4, COLOR_BLACK, COLOR_GREEN); //default selected
    attron(COLOR_PAIR(1));
    current_memory_row = 0;
    crt_offset = 0;
    current = DEC;
    crt_notification = 0;
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);


}

//how many rows we need to print all registers
static int calculateRow(int x,enum type current, int a, int size){
    int characters = a * current;         //whitespaces
    int lines = characters / x;

    return characters % x == 0 ? lines : lines +1;
}

MYTYPE readInput(type_param tpar){
    MYTYPE input;
    int x, y, l, m, len = 40, wid = 8;
    char buffer[11];
    WINDOW *w;

    getmaxyx(stdscr, y, x);

    if(x > len) l = x/2 - len/2;
    else l = len;

    if(y > wid) m = y/2 - wid/2;
    else m = wid;

    w = newwin(wid, len, m, l);
    wbkgd(w, COLOR_PAIR(2));
    wattron(w, COLOR_PAIR(2));
    echo();
    curs_set(1);

    
    do{
	werase(w);
	box(w, 0, 0);
	
	mvwprintw(w, 0, 17, "INPUT");
	wmove(w, 1, 1);
	if(tpar == MYTYPE_PARAM)
	    wprintw(w,"Input requested as integer");
	else if(tpar == MYTYPEF_PARAM)
	    wprintw(w, "Input requested as float");
	
	wmove(w, 4, 3);
	refresh();
	wrefresh(w);

	wgetnstr(w, buffer, 11);

    }while(tpar == MYTYPE_PARAM ? sscanf(buffer, "%d", &input) != 1 || \
	   notCorrectInput(buffer) : sscanf(buffer, "%f",(volatile MYTYPEF*) &input) != 1 );

    
    delwin(w);
    curs_set(0);


    return input;
    
}

void printOutput(MYTYPE out, type_param tpar){
    struct outputList *tmp, *new;
    new = (struct outputList *) malloc(sizeof(struct outputList));

    new->output = out;
    new->tpar = tpar;
    new->next = NULL;

    if(first){
	//find last item
	for(tmp = first; tmp->next; tmp = tmp->next);
	tmp->next = new;
    }
    
    else first = new;

    if(scr != OUT) crt_notification = 1;
    if(crt_offset != 0) crt_offset++;

}

static void drawPanel(void){
    int x, y, i;

    (void) y;
    
    if(scr != CPU){
	attroff(COLOR_PAIR(1));
	attron(COLOR_PAIR(2));
	mvwprintw(stdscr, 1, 1, "<F1> CPU\t");
	attroff(COLOR_PAIR(2));
	attron(COLOR_PAIR(1));
    }
    else
	mvwprintw(stdscr, 1, 1, "<F1> CPU\t");

    if(scr != MEM){
	attroff(COLOR_PAIR(1));
	attron(COLOR_PAIR(2));
	wprintw(stdscr, "<F2> MEM\t");
	attroff(COLOR_PAIR(2));
	attron(COLOR_PAIR(1));
    }
    else
	wprintw(stdscr, "<F2> MEM\t");

    if(scr != OUT){
	if(crt_notification) attron(A_BLINK);
	attroff(COLOR_PAIR(1));
	attron(COLOR_PAIR(2));
	wprintw(stdscr, "<F3> CRT\t");
	attroff(COLOR_PAIR(2));
	attron(COLOR_PAIR(1));
	attroff(A_BLINK);
    }
    else
	wprintw(stdscr, "<F3> CRT\t");

    if(current != DEC){
	attroff(COLOR_PAIR(1));
	attron(COLOR_PAIR(2));
	wprintw(stdscr, "<D> DEC\t");
	attroff(COLOR_PAIR(2));
	attron(COLOR_PAIR(1));
    }
    else
	wprintw(stdscr, "<D> DEC\t");

    if(current != HEX){
	attroff(COLOR_PAIR(1));
	attron(COLOR_PAIR(2));
	wprintw(stdscr, "<X> HEX\t");
	attroff(COLOR_PAIR(2));
	attron(COLOR_PAIR(1));
    }
    else
	wprintw(stdscr, "<X> HEX\t");
    
    if(current != BIN){
	attroff(COLOR_PAIR(1));
	attron(COLOR_PAIR(2));
	wprintw(stdscr, "<B> BIN\t");
	attroff(COLOR_PAIR(2));
	attron(COLOR_PAIR(1));
    }
    else
	wprintw(stdscr, "<B> BIN\t");
    

    getmaxyx(stdscr, y, x);

    attroff(COLOR_PAIR(1));
    attron(COLOR_PAIR(2));
    for(i = 0; i < x -75; i++)
	wprintw(stdscr, " ");
    attroff(COLOR_PAIR(2));
    attron(COLOR_PAIR(1));

}

static void printBin(WINDOW *w, MYTYPE a){
    int i;
    for(i = 0; i < sizeof(MYTYPE)*8; i++)
	if(a >> (sizeof(MYTYPE)*8 -i -1) & 1) wprintw(w, "1");
	else wprintw(w, "0");
}

//tänne säännöt joilla output on ok
static int notCorrectInput(const char *buffer){
    int negative = 0, length = strlen(buffer), i;

    if(buffer[0] == '-')
	negative = 1;

    if(length > 10 && !negative)
	return -1;

    for(i = 1; i < length; i++)
	if(!isdigit(buffer[i]))
	    return -1;

    return 0;
}

//error check for i is outside of bounds
static int isCodeArea(int line){
    if(disassembled.codes[line] == NULL)
	return 0;
    else
	return 1;
}

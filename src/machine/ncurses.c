#include <string.h>
#include <ncurses.h>
#include <ttk-15.h>
#include "machine.h"

/* 
 * describes how many characters is needed to present
 * the value of MYTYPE(F) in specific representation
 */
enum type { BIN = 32, HEX = 8, DEC = 11, EXP = 13 };
enum screentab { CPU, MEM, OUT };

static WINDOW *drawMYTYPE(MYTYPE *addr, int elems, enum type current, int x, char **arr, int size);
static WINDOW *drawMYTYPEF(MYTYPEF *addr, int elems, enum type current, int x, char **arr, int size);
static int calculateRow(int x, enum type current, int a, int size);
static int len(MYTYPE x, enum type current);
static void drawCPU(machine *m, enum type current);
static void drawMemory(machine *m, enum type current);
static void drawCRT(machine *m);

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

/*
 * main function of ncurses gui. this is executed
 * between every instruction in debugging mode
 */
void drawScreen(machine *m){
    enum screentab scr = CPU;
    enum type current = DEC;

    int ch;

    drawCPU(m, current);
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    
    //return only if we are in CPU window and enter is pressed
    while(!((ch = getch()) == 10 && scr == CPU)){
	switch (ch){
	case KEY_F(1):
	    drawCPU(m, current);
	    scr = CPU;
	    break;
	case KEY_F(2):
	    drawMemory(m, current);
	    scr = MEM;
	    break;
	case KEY_F(3):
	    drawCRT(m);
	    scr = OUT;
	    break;
	}
    }
}

static void drawMemory(machine *m, enum type current){
    int x, y, rows, cols, slots_per_row, i;
    static enum type asd = DEC;
    
    werase(stdscr);
    box(stdscr, 0, 0);
    mvwprintw(stdscr, 1, 1, "<F1> CPU\t<F2> memory\t<F3> CRT");

    getmaxyx(stdscr, y, x);

    rows = y -3; //borders and guideline
    cols = (x -2) / (current + 5); //5?
    (void) cols;

    slots_per_row = (x -2) / (current + 5);

    wmove(stdscr, 1, 1);
    for(i = 0; i < slots_per_row * rows; i++){
	if(i % slots_per_row == 0){
	    getyx(stdscr, y, x);
	    wmove(stdscr, y +1, 1);
	}
	wprintw(stdscr, "%d: %d ", i, m->mem[i]);
	getyx(stdscr, y, x);
	wmove(stdscr, y, x +current -len(m->mem[i], current) -len(i, asd));
    }
}

static void drawCRT(machine *m){
    werase(stdscr);
    box(stdscr, 0, 0);
    mvwprintw(stdscr, 1, 1, "<F1> CPU\t<F2> memory\t<F3> CRT");
    

}

static void drawCPU(machine *m, enum type current){
    int y, x, pos = 2;
    static WINDOW *registers, *MMU, *CU, *ALU, *FPU;

    //clear all output from last draw
    clear();
    delwin(registers);
    delwin(MMU);
    delwin(CU);
    delwin(ALU);
    delwin(FPU);
    
    //macro
    getmaxyx(stdscr, y, x);

    //create subwindows for every part of cpu
    registers = drawMYTYPE(m->regs, 8, current, x -2, (char**)registers_array, 3);
    MMU = drawMYTYPE(&m->mmu->limit, 4, current, x -2, (char**)MMU_array, 4);
    CU = drawMYTYPE(&m->cu->tr, 4, current, x -2, (char**)CU_array, 3);
    ALU = drawMYTYPE(&m->alu->in1, 3, current, x -2, (char**)ALU_array, 4);
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

    box(stdscr, 0, 0);
    mvwprintw(stdscr, 1, 1, "<F1> CPU\t<F2> memory\t<F3> CRT");

    //print subwindows and main window
    refresh();    
    wrefresh(registers);
    wrefresh(MMU);
    wrefresh(CU);
    wrefresh(ALU);
    wrefresh(FPU);
    
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
static WINDOW *drawMYTYPE(MYTYPE *addr, int elems, enum type current, int x, char *arr[], int size){
    WINDOW *w;
    int row, regsperrow, i, j, currentx, currenty, count, length;

    //how many rows we need to dras all elements
    // -2 for window borded
    row = calculateRow(x -2, current +size +2 , elems, size);
    //how many elements could be drawn to one line
    regsperrow = (x -2) / (current +size +2);
    
    w = newwin(row +2, x, 0, 0);              // +2 for window borded

    box(w, 0, 0);

    wmove(w, 1, 1);
    count = 0;
    
    for(i = 0; i < row; i++){
	for(j = 0; j < regsperrow && count < elems; j++){

	    //print one item
	    if(current == DEC)
		wprintw(w, "%s %d ", arr[count], addr[count]);
	    else if(current == HEX)
		wprintw(w, "%s %x ", arr[count], addr[count]);

	    //move cursor to start of second item
	    length = len(addr[count++], current);
	    getyx(w, currenty, currentx);
	    wmove(w, currenty, currentx +current +size -length -strlen(arr[count -1]));
	}
	//move cursor to beginning of second line
	getyx(w, currenty, currentx);
	wmove(w, currenty +1, 1);
    }
    //is this needed?
    wmove(w, 0, 0);

    return w;
}
static WINDOW *drawMYTYPEF(MYTYPEF *addr, int elems, enum type current, int x, char *arr[], int size){
    WINDOW *w;
    int row, regsperrow, i, j, currentx, currenty, count, length;

    if(current == DEC) current = EXP;

    row = calculateRow(x -2, current +size +2, elems, size);
    regsperrow = (x -2) / (current +size +2);
    
    w = newwin(row +2, x, 0, 0);

    box(w, 0, 0);

    wmove(w, 1, 1);
    count = 0;
    for(i = 0; i < row; i++){
	for(j = 0; j < regsperrow && count < elems; j++){

	    if(current == EXP)
		wprintw(w, "%s %g ", arr[count], addr[count]);
	    else if(current == HEX)
		wprintw(w, "%s, %x ", arr[count], addr[count]);

	    length = len(addr[count++], current);
	    getyx(w, currenty, currentx);
	    wmove(w, currenty, currentx + current -length -size);
	}
	getyx(w, currenty, currentx);
	wmove(w, currenty +1, 1);
    }

    wmove(w, 0, 0);

    return w;
}

static int len(MYTYPE x, enum type current){
    int factor;
    int len = 1;

    if(current == HEX) factor = 16;
    if(current == DEC) factor = 10;
    if(current == EXP) factor = 12;

    //possible owerflow!
    if(x < 0 && (current == DEC || current == EXP)){
	x = x * (-1);
	len++;
    }

    for(; x >= factor; x /= factor)
	len++;

    return len;
}

void initScreen(void){
    initscr();
    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
}

//how many rows we need to print all registers
static int calculateRow(int x, enum type current, int a, int size){
    int characters = a * current;         //whitespaces
    int lines = characters / x;

    return characters % x == 0 ? lines : lines +1;
}

MYTYPE readInput(void){
    MYTYPE input;
    int x, y, l, m, len = 40, wid = 8;
    WINDOW *w;

    getmaxyx(stdscr, y, x);

    if(x > len) l = x/2 - len/2;
    else l = len;

    if(y > wid) m = y/2 - wid/2;
    else m = wid;

    w = newwin(wid, len, m, l);
    echo();
    curs_set(1);
    
    do{
	werase(w);
	box(w, 0, 0);
	
	mvwprintw(w, 1, 1, "                 INPUT");
	wmove(w, 4, 3);
	refresh();
	wrefresh(w);
    }while(wscanw(w, "%d", &input) != 1);

    
    delwin(w);
    curs_set(0);

    return input;
    
}

 void printOutput(MYTYPE out){
 }

#include <ncurses.h>
#include <ttk-15.h>
#include "machine.h"

/* 
 * describes how many characters is needed to present
 * the value of MYTYPE(F) in specific representation
 */
enum type { BIN = 32, HEX = 8, DEC = 11, EXP = 13 };

static WINDOW *drawMYTYPE(MYTYPE *addr, int elems, enum type current, int x);
static WINDOW *drawMYTYPEF(MYTYPEF *addr, int elems, enum type current, int x);
static int calculateRow(int x, enum type current, int a);
static int len(MYTYPE x, enum type current);

/*
 * main function of ncurses gui. this is executed
 * between every instruction in debugging mode
 */
void drawScreen(machine *m){
    int y, x, pos = 1;
    enum type current = DEC;
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
    registers = drawMYTYPE(m->regs, 8, current, x -2);
    MMU = drawMYTYPE(&m->mmu->limit, 4, current, x -2);
    CU = drawMYTYPE(&m->cu->tr, 4, current, x -2);
    ALU = drawMYTYPE(&m->alu->in1, 3, current, x -2);
    FPU = drawMYTYPEF(&m->fpu->in1, 3, current, x -2);
    
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

    //print subwindows and main window
    refresh();    
    wrefresh(registers);
    wrefresh(MMU);
    wrefresh(CU);
    wrefresh(ALU);
    wrefresh(FPU);
    
    noecho();
    getch();
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
static WINDOW *drawMYTYPE(MYTYPE *addr, int elems, enum type current, int x){
    WINDOW *w;
    int row, regsperrow, i, j, currentx, currenty, count, length;

    //how many rows we need to dras all elements
    row = calculateRow(x -2, current, elems); // -2 for window borded
    //how many elements could be drawn to one line
    regsperrow = (x -2) / (current);
    
    w = newwin(row +2, x, 0, 0);              // +2 for window borded

    box(w, 0, 0);

    wmove(w, 1, 1);
    count = 0;
    
    for(i = 0; i < row; i++){
	for(j = 0; j < regsperrow && count < elems; j++){

	    //print one item
	    if(current == DEC)
		wprintw(w, "%d ", addr[count]);
	    else if(current == HEX)
		wprintw(w, "%x ", addr[count]);

	    //move cursor to start of second item
	    length = len(addr[count++], current);
	    getyx(w, currenty, currentx);
	    wmove(w, currenty, currentx + current -length);
	}
	//move cursor to beginning of second line
	getyx(w, currenty, currentx);
	wmove(w, currenty +1, 1);
    }
    //is this needed?
    wmove(w, 0, 0);

    return w;
}
static WINDOW *drawMYTYPEF(MYTYPEF *addr, int elems, enum type current, int x){
    WINDOW *w;
    int row, regsperrow, i, j, currentx, currenty, count, length;

    if(current == DEC) current = EXP;

    row = calculateRow(x -2, current, elems);
    regsperrow = (x -2) / (current);
    
    w = newwin(row +2, x, 0, 0);

    box(w, 0, 0);

    wmove(w, 1, 1);
    count = 0;
    for(i = 0; i < row; i++){
	for(j = 0; j < regsperrow && count < elems; j++){

	    if(current == EXP)
		wprintw(w, "%g ", addr[count]);
	    else if(current == HEX)
		wprintw(w, "%x ", addr[count]);

	    length = len(addr[count++], current);
	    getyx(w, currenty, currentx);
	    wmove(w, currenty, currentx + current -length);
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
}

//how many rows we need to print all registers
static int calculateRow(int x, enum type current, int a){
    int characters = a * (current +1);             //whitespace
    int lines = characters / x;
    
    return characters % x == 0 ? lines : lines +1;
}




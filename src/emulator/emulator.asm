section .data
    instr: dq NOP,STORE,LOAD,IN,OUT,INV,INV,INV,\
            INV,INV,INV,INV,INV,INV,INV,INV,\
            INV,ADD,SUB,MUL,DIV,MOD,AND,OR,\
            XOR,SHL,SHR,NOT,SHRA,INV,INV,COMP,\
            JUMP,JNEG,JZER,JPOS,JNNEG,JNZER,JNPOS,JLES,\
            JEQU,JGRE,JNLES,JNEQU,JNGRE,INV,INV,INV,\
            INV,CALL,EXIT,PUSH,POP,PUSHR,POPR,INV,\
            INV,INV,INV,INV,INV,INV,INV,INV,\
            INV,INV,INV,INV,INV,INV,INV,INV,\
            INV,INV,INV,INV,INV,INV,INV,INV,\
            INV,INV,INV,INV,INV,INV,INV,INV,\
            INV,INV,INV,INV,INV,INV,INV,INV,\
            INV,INV,INV,INV,INV,INV,INV,INV,\
            INV,INV,INV,INV,INV,INV,INV,INV,\
            SVC
;   the ttk-15 binary goes here
data incbin "a.out.b15",4 ; we skip the 4 byte header
regjumptable: dq reg0,reg1,reg2,reg3,reg4,reg5,reg6,reg7
toregjumptable: dq toreg0,toreg1,toreg2,toreg3,toreg4,toreg5,toreg6,toreg7
stack times 512 dd 0
stackptr equ stack-data
section .text
default rel
global _start
; esi is a representation of flags GEL in ttk machine
; r14d is the frame pointer
; r15d is the stack pointer
; r8 - r13 are r0-r5
; edx is used for the right hand operand. a.k.a address part + index register
; edi is used for the left hand operand, which is always a register
; ax contains the instruction, mode, register and index register values
_start:
    xor ebx,ebx
    xor ecx,ecx
    mov r15d, stackptr
    nextinstr:
    ; get instructions
    mov eax,[ecx*0x4+data]
    xor edx,edx
    ; address part to dx
    mov dx, ax
    shr eax,16
    ; check mode
    ; mode = 2
    test eax,0x10
    jnz mode_2
    ; mode = 1
    test eax,0x08
    jnz mode_1
    jmp mode_end
    mode_2:
        mov edx,dword [data+edx*0x4]
    mode_1:
        mov edx,dword [data+edx*0x4]
    mode_end:
    mov bl,ah
    inc ecx
    ; execute instruction
    ; set edi to contain register value
    jmp get_register
    register_recieved:
    push rdi
    jmp get_index_register
    index_register_recieved:
    add edx,edi
    pop rdi
    jmp [instr+ebx*8]
    instruction_done:
    ; move value from edi to register
    jmp put_register
    register_put:
    jmp nextinstr
    ; exit 0
    end:
    mov eax,0x3c
    mov edi,0x0
    syscall
;;;;;;;;;;;;;;;;;;;;;
get_register:
    push rax
    shr eax,0x5
    and eax,0x7
    call [regjumptable+rax*0x8]
    pop rax
    jmp register_recieved
get_index_register:
    push rax
    and eax,0x7
    test eax,eax
    jz no_index
        call [regjumptable+rax*0x8]
        pop rax
        jmp index_register_recieved
    no_index:
        xor edi,edi
        pop rax
        jmp index_register_recieved
put_register:
    shr eax,0x5
    and eax,0x7
    call [toregjumptable+rax*0x8]
    jmp register_put
;; get contents of register
reg0:
    mov edi,r8d
    ret
reg1:
    mov edi,r9d
    ret
reg2:
    mov edi,r10d
    ret
reg3:
    mov edi,r11d
    ret
reg4:
    mov edi,r12d
    ret
reg5:
    mov edi,r13d
    ret
reg6:
    mov edi,r14d
    ret
reg7:
    mov edi,r15d
    ret
;; put contents to register
toreg0:
    mov r8d,edi
    ret
toreg1:
    mov r9d,edi
    ret
toreg2:
    mov r10d,edi
    ret
toreg3:
    mov r11d,edi
    ret
toreg4:
    mov r12d,edi
    ret
toreg5:
    mov r13d,edi
    ret
toreg6:
    mov r14d,edi
    ret
toreg7:
    mov r15d,edi
    ret
NOP:
    jmp instruction_done
STORE:
    mov [data+edx*0x4],edi
    jmp instruction_done
LOAD:
    mov edi, edx
    jmp instruction_done
IN:
    jmp instruction_done
OUT:
    jmp instruction_done
ADD:
    add edi,edx
    jmp instruction_done
SUB:
    sub edi,edx
    jmp instruction_done
MUL:
    push rax
    push rdx
    mov eax,edi
    mul edx
    mov edi,eax
    pop rdx
    pop rax
    jmp instruction_done
DIV:
    jmp instruction_done
MOD:
    jmp instruction_done
AND:
    and edi,edx
    jmp instruction_done
OR:
    or edi,edx
    jmp instruction_done
XOR:
    xor edi,edx
    jmp instruction_done
SHL:
    push rcx
    mov ecx,edx
    sal edi,cl
    pop rcx
    jmp instruction_done
SHR:
    push rcx
    mov ecx,edx
    sar edi,cl
    pop rcx
    jmp instruction_done
NOT:
    xor edi,0xffffffff
    jmp instruction_done
SHRA:
; todo
    jmp instruction_done
COMP:
    xor esi,esi
    mov esi,edi
    sub esi,edx
    jmp instruction_done
JUMP:
    mov ecx,edx
    jmp instruction_done
JNEG:
    cmp edi,0x0
    jnl instruction_done
    mov ecx,edx
    jmp instruction_done
JZER:
    cmp edi,0x0
    jnz instruction_done
    mov ecx,edx
    jmp instruction_done
JPOS:
    cmp edi,0x0
    jng instruction_done
    mov ecx,edx
    jmp instruction_done
JNNEG:
    cmp edi,0x0
    jl instruction_done
    mov ecx,edx
    jmp instruction_done
JNZER:
    cmp edi,0x0
    jz instruction_done
    mov ecx,edx
    jmp instruction_done
JNPOS:
    cmp edi,0x0
    jg instruction_done
    mov ecx,edx
    jmp instruction_done
JLES:
    cmp esi,0x0
    jnl instruction_done
    mov ecx,edx
    jmp instruction_done
JEQU:
    cmp esi,0x0
    jne instruction_done
    mov ecx,edx
    jmp instruction_done
JGRE:
    cmp esi,0x0
    jng instruction_done
    mov ecx,edx
    jmp instruction_done
JNLES:
    cmp esi,0x0
    jl instruction_done
    mov ecx,edx
    jmp instruction_done
JNEQU:
    cmp esi,0x0
    je instruction_done
    mov ecx,edx
    jmp instruction_done
JNGRE:
    cmp esi,0x0
    jg instruction_done
    mov ecx,edx
    jmp instruction_done
CALL:
    ; pushing values
    mov [r15d+data+4],ecx
    mov [r15d+data+8],r14d
    add r15d,0x8
    ; make new frame pointer
    mov r14d,r15d
    mov ecx,edx
    jmp instruction_done
EXIT:
    ; popping values
    sub r15d,0x8
    mov ecx,[r15d+data+4]
    mov r14d,[r15d+data+8]
    mov eax,0x4
    mul edx
    sub r15d,eax
    jmp instruction_done
PUSH:
    add r15d,0x4
    mov [r15d+data],rdi
    jmp instruction_done
POP:
    mov rdi,[r15d+data]
    sub r15d,0x4
    jmp instruction_done
PUSHR:
    mov [r15d+data+4],r8d
    mov [r15d+data+8],r9d
    mov [r15d+data+12],r10d
    mov [r15d+data+16],r11d
    mov [r15d+data+20],r12d
    mov [r15d+data+24],r13d
    add r15d,24
    jmp instruction_done
POPR:
    sub r15d,24
    mov r8d,[r15d+data+4]
    mov r9d,[r15d+data+8]
    mov r10d,[r15d+data+12]
    mov r11d,[r15d+data+16]
    mov r12d,[r15d+data+20]
    mov r13d,[r15d+data+24]
    jmp instruction_done
SVC:
    ; halt 
    cmp edx,0x0B
    je end
    cmp edx,0x80
    jne instruction_done
    ; for now only write instruction
    cmp r9d,0x4
    jne end
    push rax
    push rcx
    push rbx
    push rdx
    mov eax,r9d
    mov ebx,r10d
    lea ecx,[r11d*0x4+data]
    mov edx,r12d
    int 0x80
    pop rdx
    pop rbx
    pop rcx
    pop rax
    jmp instruction_done
INV:
    jmp instruction_done

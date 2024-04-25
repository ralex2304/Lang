; Program start

section .text

extern printf
extern scanf
global _start

_start:
        enter 0, 0
        ; func call: министерство
        call ___func_0
        ; func call end

        leave
        mov rax, 0x3c
        cvttsd2si rdi, xmm0
        syscall

; =========================== Function definition =========================
; func name: министерство
___func_0:
        enter 0x0058, 0; this place is patched in the end of function definition

        sub rsp, 8
        mov rdx, 0x0
        mov qword [rsp], rdx
        lea rcx, [rbp - 8 - 0]
        pop qword [rcx]

        sub rsp, 8
        mov rdx, 0x3ff0000000000000
        mov qword [rsp], rdx
        lea rcx, [rbp - 8 - 8]
        pop qword [rcx]

        sub rsp, 8
        mov rdx, 0x4000000000000000
        mov qword [rsp], rdx
        lea rcx, [rbp - 8 - 16]
        pop qword [rcx]

        sub rsp, 8
        mov rdx, 0x4008000000000000
        mov qword [rsp], rdx
        lea rcx, [rbp - 8 - 24]
        pop qword [rcx]

        sub rsp, 8
        mov rdx, 0x4010000000000000
        mov qword [rsp], rdx
        lea rcx, [rbp - 8 - 32]
        pop qword [rcx]

        sub rsp, 8
        mov rdx, 0x4014000000000000
        mov qword [rsp], rdx
        lea rcx, [rbp - 8 - 40]
        pop qword [rcx]

        sub rsp, 8
        mov rdx, 0x4018000000000000
        mov qword [rsp], rdx
        lea rcx, [rbp - 8 - 48]
        pop qword [rcx]

        sub rsp, 8
        mov rdx, 0x0
        mov qword [rsp], rdx
        ; var assignment: индекс
        pop qword [rbp - 8 - 80]

        ; while begin
___while_3_begin:
        push qword [rbp - 8 - 80]
        ; prepost oper
        push qword [rbp - 8 - 80]
        sub rsp, 8
        mov rdx, 0x3ff0000000000000
        mov qword [rsp], rdx
        movsd xmm2, [rsp]
        movsd xmm1, [rsp + 8]
        add rsp, 8
        addsd xmm1, xmm2
        movsd [rsp], xmm1

        pop qword [rbp - 8 - 80]

        lea rcx, [rbp - 8 - 0]
        cvtsd2si rdx, [rsp]
        add rsp, 8
        shl rdx, 3
        sub rcx, rdx 
        push qword [rcx]
        sub rsp, 8
        mov rdx, 0x4014000000000000
        mov qword [rsp], rdx
        ; operands comparison: op1 < op2
        movsd xmm2, [rsp]
        movsd xmm1, [rsp + 8]
        add rsp, 8
        movsd xmm3, xmm1
        subsd xmm3, xmm2
        movsd [rsp], xmm3
        mov rdx, -1 >> 1 ; 0x7FFFFFFFFFFFFFFF
        and qword [rsp], rdx
        movsd xmm3, [rsp]
        sub rsp, 8
        mov rdx, 0x3eb0c6f7a0b5ed8d
        mov qword [rsp], rdx
        movsd xmm4, [rsp]
        add rsp, 8
        ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2); xmm4 - EPSILON

        comisd xmm3, xmm4 ; fabs(op1 - op2) {'<' | '>'} EPSILON
        jc ___compare_1_false

        comisd xmm1, xmm2 ; op1 {'<' | '>'} op2
        jnc ___compare_1_false

        sub rsp, 8
        mov rdx, 0x3ff0000000000000
        mov qword [rsp], rdx
        jmp ___compare_1_end

___compare_1_false:
        sub rsp, 8
        mov rdx, 0x0
        mov qword [rsp], rdx
___compare_1_end:

        ; while clause check
        sub rsp, 8
        mov rdx, 0x3eb0c6f7a0b5ed8d
        mov qword [rsp], rdx
        mov rdx, -1 >> 1 ; 0x7FFFFFFFFFFFFFFF
        and qword [rsp + 8], rdx
        movsd xmm1, [rsp + 8]
        movsd xmm2, [rsp]
        add rsp, 16
        comisd xmm1, xmm2
        jc ___while_3_end

        push qword [rbp - 8 - 80]
        lea rcx, [rbp - 8 - 0]
        cvtsd2si rdx, [rsp]
        add rsp, 8
        shl rdx, 3
        sub rcx, rdx 
        push qword [rcx]
        movsd xmm0, [rsp]
        add rsp, 8
        call doubleio_out

        jmp ___while_3_begin

___while_3_end:
        ; while end

        leave
        ret
; ------------------------- Function definition end -----------------------


        %include "doubleiolib.nasm"

section .data

GLOBAL_SECTION: times 0 dq 0

section .rodata

PRINTF_DOUBLE_FMT: db '%', 'l', 'g', 0x0a, 0
SCANF_DOUBLE_FMT:  db '%', 'l', 'f', 0

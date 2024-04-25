; Program start

section .text

extern printf
extern scanf
global _start

_start:
        enter 0, 0
        ; func call: остров_в_океане
        call ___func_0
        ; func call end

        leave
        mov rax, 0x3c
        cvttsd2si rdi, xmm0
        syscall

; =========================== Function definition =========================
; func name: остров_в_океане
___func_0:
        enter 0x0010, 0; this place is patched in the end of function definition

        sub rsp, 8
        mov rdx, 0x4014000000000000
        mov qword [rsp], rdx
        ; var assignment: переменная
        pop qword [rbp - 8 - 0]

        ; scope begin
        push qword [rbp - 8 - 0]
        movsd xmm0, [rsp]
        add rsp, 8
        call doubleio_out

        sub rsp, 8
        mov rdx, 0x4024000000000000
        mov qword [rsp], rdx
        ; var assignment: переменная
        pop qword [rbp - 8 - 8]

        push qword [rbp - 8 - 8]
        movsd xmm0, [rsp]
        add rsp, 8
        call doubleio_out

        sub rsp, 8
        mov rdx, 0x4024000000000000
        mov qword [rsp], rdx
        push qword [rbp - 8 - 8]
        ; swap last stk vals
        pop rdx
        pop r8
        push rdx
        push r8

        movsd xmm2, [rsp]
        movsd xmm1, [rsp + 8]
        add rsp, 8
        addsd xmm1, xmm2
        movsd [rsp], xmm1

        ; var assignment: переменная
        pop qword [rbp - 8 - 8]

        push qword [rbp - 8 - 8]
        movsd xmm0, [rsp]
        add rsp, 8
        call doubleio_out

        ; scope end
        push qword [rbp - 8 - 0]
        movsd xmm0, [rsp]
        add rsp, 8
        call doubleio_out

        sub rsp, 8
        mov rdx, 0x0
        mov qword [rsp], rdx
        movsd xmm0, [rsp]
        add rsp, 8
        leave
        ret

        leave
        ret
; ------------------------- Function definition end -----------------------


        %include "doubleiolib.nasm"

section .data

GLOBAL_SECTION: times 0 dq 0

section .rodata

PRINTF_DOUBLE_FMT: db '%', 'l', 'g', 0x0a, 0
SCANF_DOUBLE_FMT:  db '%', 'l', 'f', 0

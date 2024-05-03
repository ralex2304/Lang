___ir_block_1:
; Program start

section .text

extern printf
extern scanf
global _start

_start:
                enter 0, 0
___ir_block_2:
                call ___func_4
                ; func call end

___ir_block_3:
                leave
                mov rax, 0x3c
                cvttsd2si rdi, xmm0
                syscall

___ir_block_4:
; =========================== Function definition =========================
___func_4:
                enter 88, 0

___ir_block_5:
                sub rsp, 8
                mov rdx, 0x0 ; 0
                mov qword [rsp], rdx
___ir_block_6:
                lea rcx, qword [rbp - 8 - 0]
___ir_block_7:
                mov rdx, [rsp]
                add rsp, 8
                mov qword [rcx], rdx
___ir_block_8:
                sub rsp, 8
                mov rdx, 0x3ff0000000000000 ; 1
                mov qword [rsp], rdx
___ir_block_9:
                lea rcx, qword [rbp - 8 - 8]
___ir_block_10:
                mov rdx, [rsp]
                add rsp, 8
                mov qword [rcx], rdx
___ir_block_11:
                sub rsp, 8
                mov rdx, 0x4000000000000000 ; 2
                mov qword [rsp], rdx
___ir_block_12:
                lea rcx, qword [rbp - 8 - 16]
___ir_block_13:
                mov rdx, [rsp]
                add rsp, 8
                mov qword [rcx], rdx
___ir_block_14:
                sub rsp, 8
                mov rdx, 0x4008000000000000 ; 3
                mov qword [rsp], rdx
___ir_block_15:
                lea rcx, qword [rbp - 8 - 24]
___ir_block_16:
                mov rdx, [rsp]
                add rsp, 8
                mov qword [rcx], rdx
___ir_block_17:
                sub rsp, 8
                mov rdx, 0x4010000000000000 ; 4
                mov qword [rsp], rdx
___ir_block_18:
                lea rcx, qword [rbp - 8 - 32]
___ir_block_19:
                mov rdx, [rsp]
                add rsp, 8
                mov qword [rcx], rdx
___ir_block_20:
                sub rsp, 8
                mov rdx, 0x4014000000000000 ; 5
                mov qword [rsp], rdx
___ir_block_21:
                lea rcx, qword [rbp - 8 - 40]
___ir_block_22:
                mov rdx, [rsp]
                add rsp, 8
                mov qword [rcx], rdx
___ir_block_23:
                sub rsp, 8
                mov rdx, 0x4018000000000000 ; 6
                mov qword [rsp], rdx
___ir_block_24:
                lea rcx, qword [rbp - 8 - 48]
___ir_block_25:
                mov rdx, [rsp]
                add rsp, 8
                mov qword [rcx], rdx
___ir_block_26:
                sub rsp, 8
                mov rdx, 0x0 ; 0
                mov qword [rsp], rdx
___ir_block_27:
                mov rdx, [rsp]
                add rsp, 8
                mov qword [rbp - 8 - 80], rdx
___ir_block_28:
___ir_block_29:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 80]
                mov qword [rsp], rdx
___ir_block_30:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 80]
                mov qword [rsp], rdx
___ir_block_31:
                sub rsp, 8
                mov rdx, 0x3ff0000000000000 ; 1
                mov qword [rsp], rdx
___ir_block_32:
                movsd xmm0, [rsp + 8]
                addsd xmm0, qword [rsp]
                add rsp, 8
                movsd qword [rsp], xmm0
___ir_block_33:
                mov rdx, [rsp]
                add rsp, 8
                mov qword [rbp - 8 - 80], rdx
___ir_block_34:
                lea rcx, qword [rbp - 8 - 0]
___ir_block_35:
                cvtsd2si rdx, [rsp]
                add rsp, 8
                shl rdx, 3
                sub rcx, rdx
___ir_block_36:
                sub rsp, 8
                mov rdx, qword [rcx]
                mov qword [rsp], rdx
___ir_block_37:
                sub rsp, 8
                mov rdx, 0x4014000000000000 ; 5
                mov qword [rsp], rdx
___ir_block_38:
                ; operands comparison: op1 < op2
                movsd xmm2, [rsp]
                movsd xmm1, [rsp + 8]
                movsd xmm3, xmm1
                subsd xmm3, xmm2
                movsd [rsp], xmm3
                mov rdx, -1 >> 1
                and qword [rsp], rdx
                movsd xmm3, [rsp]
                add rsp, 16
                ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2)

                comisd xmm3, [EPSILON] ; fabs(op1 - op2) {'<' | '>'} EPSILON
                jc ___compare_38_false

                comisd xmm1, xmm2 ; op1 {'<' | '>'} op2
                jnc ___compare_38_false

                sub rsp, 8
                mov rdx, 0x3ff0000000000000 ; 1
                mov qword [rsp], rdx
                jmp ___compare_38_end

___compare_38_false:
                sub rsp, 8
                mov rdx, 0x0 ; 0
                mov qword [rsp], rdx
___compare_38_end:

___ir_block_39:
                mov rdx, -1 >> 1
                and qword [rsp], rdx
                movsd xmm1, qword [rsp]
                add rsp, 8
                comisd xmm1, [EPSILON]
___ir_block_40:
                jc ___ir_block_47
___ir_block_41:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 80]
                mov qword [rsp], rdx
___ir_block_42:
                lea rcx, qword [rbp - 8 - 0]
___ir_block_43:
                cvtsd2si rdx, [rsp]
                add rsp, 8
                shl rdx, 3
                sub rcx, rdx
___ir_block_44:
                sub rsp, 8
                mov rdx, qword [rcx]
                mov qword [rsp], rdx
___ir_block_45:
                movsd xmm0, qword [rsp]
                add rsp, 8
                call doubleio_out
___ir_block_46:
                jmp ___ir_block_28
___ir_block_47:
___ir_block_48:
                leave
                pxor xmm0, xmm0
                ret
; ------------------------- Function definition end -----------------------


___ir_block_49:
%include "doubleiolib.nasm"

section .data

GLOBAL_SECTION: times 0 dq 0

section .rodata

EPSILON: dq 0x3eb0c6f7a0b5ed8d ; 1e-06


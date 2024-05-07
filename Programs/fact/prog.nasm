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
                enter 0, 0

___ir_block_5:
                call doubleio_in
                movq qword [rsp - 8 - 16 - 0], xmm0
___ir_block_6:
___ir_block_7:
                call ___func_14
                ; func call end

___ir_block_8:
___ir_block_9:
                movq xmm0, xmm0
                call doubleio_out
___ir_block_10:
___ir_block_11:
                mov qword rdx, 0x0 ; 0
                movq xmm0, rdx
___ir_block_12:
                leave
                ret

___ir_block_13:
                leave
                pxor xmm0, xmm0
                ret
; ------------------------- Function definition end -----------------------


___ir_block_14:
; =========================== Function definition =========================
___func_14:
                enter 8, 0

___ir_block_15:
___ir_block_16:
___ir_block_17:
                ; operands comparison: op1 > op2
                movq xmm1, qword [rbp - 8 - 0]
                mov rdx, 0x3ff0000000000000
                movq xmm2, rdx
                add rsp, 18446744073709551608
                movq xmm3, xmm1
                subsd xmm3, xmm2
                andpd xmm3, [DOUBLE_NEG_CONST]
                ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2)

                comisd xmm3, [EPSILON] ; fabs(op1 - op2) {'<' | '>'} EPSILON
                jc ___compare_17_false

                comisd xmm1, xmm2 ; op1 {'<' | '>'} op2
                jc ___compare_17_false

                mov rdx, 0x3ff0000000000000 ; 1
                jmp ___compare_17_end

___compare_17_false:
                mov rdx, 0x0 ; 0
___compare_17_end:
                mov qword [rsp], rdx

___ir_block_18:
                mov rdx, -1 >> 1
                and qword [rsp], rdx
                movq xmm1, qword [rsp]
                add rsp, 8
                comisd xmm1, [EPSILON]
___ir_block_19:
                jc ___ir_block_30
___ir_block_20:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 0]
                mov qword [rsp], rdx
___ir_block_21:
                sub rsp, 8
                mov rdx, 0x3ff0000000000000 ; 1
                mov qword [rsp], rdx
___ir_block_22:
                movq xmm0, [rsp + 8]
                subsd xmm0, qword [rsp]
                add rsp, 16
                movq qword [rsp - 8 - 16 - 0], xmm0
___ir_block_23:
___ir_block_24:
                call ___func_14
                ; func call end

___ir_block_25:
___ir_block_26:
___ir_block_27:
                mulsd xmm0, qword [rbp - 8 - 0]
___ir_block_28:
___ir_block_29:
                leave
                ret

___ir_block_30:
___ir_block_31:
___ir_block_32:
                mov qword rdx, 0x3ff0000000000000 ; 1
                movq xmm0, rdx
___ir_block_33:
                leave
                ret

___ir_block_34:
                leave
                pxor xmm0, xmm0
                ret
; ------------------------- Function definition end -----------------------


___ir_block_35:
%include "doubleiolib.nasm"

section .data

GLOBAL_SECTION: times 0 dq 0

section .rodata

align 8
EPSILON: dq 0x3eb0c6f7a0b5ed8d ; 1e-06
align 16
DOUBLE_NEG_CONST: dq -1 >> 1, 0


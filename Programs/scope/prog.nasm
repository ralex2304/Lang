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
                enter 16, 0

___ir_block_5:
                sub rsp, 8
                mov rdx, 0x4014000000000000 ; 5
                mov qword [rsp], rdx
___ir_block_6:
                mov rdx, [rsp]
                add rsp, 8
                mov qword [rbp - 8 - 0], rdx
___ir_block_7:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 0]
                mov qword [rsp], rdx
___ir_block_8:
                movsd xmm0, qword [rsp]
                add rsp, 8
                call doubleio_out
___ir_block_9:
                sub rsp, 8
                mov rdx, 0x4024000000000000 ; 10
                mov qword [rsp], rdx
___ir_block_10:
                mov rdx, [rsp]
                add rsp, 8
                mov qword [rbp - 8 - 8], rdx
___ir_block_11:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 8]
                mov qword [rsp], rdx
___ir_block_12:
                movsd xmm0, qword [rsp]
                add rsp, 8
                call doubleio_out
___ir_block_13:
                sub rsp, 8
                mov rdx, 0x4024000000000000 ; 10
                mov qword [rsp], rdx
___ir_block_14:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 8]
                mov qword [rsp], rdx
___ir_block_15:
                movsd xmm1, [rsp + 8]
                movsd xmm2, qword [rsp]
                movsd [rsp + 8], xmm2
                movsd qword [rsp], xmm1
___ir_block_16:
                movsd xmm0, [rsp + 8]
                addsd xmm0, qword [rsp]
                add rsp, 8
                movsd qword [rsp], xmm0
___ir_block_17:
                mov rdx, [rsp]
                add rsp, 8
                mov qword [rbp - 8 - 8], rdx
___ir_block_18:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 8]
                mov qword [rsp], rdx
___ir_block_19:
                movsd xmm0, qword [rsp]
                add rsp, 8
                call doubleio_out
___ir_block_20:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 0]
                mov qword [rsp], rdx
___ir_block_21:
                movsd xmm0, qword [rsp]
                add rsp, 8
                call doubleio_out
___ir_block_22:
                sub rsp, 8
                mov rdx, 0x0 ; 0
                mov qword [rsp], rdx
___ir_block_23:
                movsd xmm0, [rsp]
                add rsp, 8
___ir_block_24:
                leave
                ret

___ir_block_25:
                leave
                pxor xmm0, xmm0
                ret
; ------------------------- Function definition end -----------------------


___ir_block_26:
%include "doubleiolib.nasm"

section .data

GLOBAL_SECTION: times 0 dq 0

section .rodata

EPSILON: dq 0x3eb0c6f7a0b5ed8d ; 1e-06


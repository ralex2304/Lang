___ir_block_1:                            ; offs: 0x120
%include "doubleiolib.nasm"

section .rodata

align 8
EPSILON: dq 0x3eb0c6f7a0b5ed8d
align 16
DOUBLE_NEG_CONST: dq -1 >> 1, 0

section .data

GLOBAL_SECTION: times 0 dq 0

; Program start

section .text

global _start

_start:
                enter 0, 0
___ir_block_2:                            ; offs: 0x403234
                call ___func_4
                ; func call end

___ir_block_3:                            ; offs: 0x403239
                leave
                mov rax, 0x3c
                cvttsd2si rdi, xmm0
                syscall

___ir_block_4:                            ; offs: 0x40324b
; =========================== Function definition =========================
___func_4:
                enter 16, 0
___ir_block_5:                            ; offs: 0x40324f
___ir_block_6:                            ; offs: 0x40324f
                mov rax, 0x4014000000000000
                mov qword [rbp - 8 - 0], rax

___ir_block_7:                            ; offs: 0x403260
___ir_block_8:                            ; offs: 0x403260
                movq xmm0, qword [rbp - 8 - 0]
                call doubleio_out
___ir_block_9:                            ; offs: 0x40326d
___ir_block_10:                            ; offs: 0x40326d
                mov rax, 0x4024000000000000
                mov qword [rbp - 8 - 8], rax

___ir_block_11:                            ; offs: 0x40327e
___ir_block_12:                            ; offs: 0x40327e
                movq xmm0, qword [rbp - 8 - 8]
                call doubleio_out
___ir_block_13:                            ; offs: 0x40328b
                sub rsp, 8
                mov rax, 0x4024000000000000
                mov qword [rsp], rax

___ir_block_14:                            ; offs: 0x4032a0
                sub rsp, 8
                mov rax, qword [rbp - 8 - 8]

                mov qword [rsp], rax

___ir_block_15:                            ; offs: 0x4032b2
                movq xmm1, [rsp + 8]
                movq xmm2, qword [rsp]
                movq [rsp + 8], xmm2
                movq qword [rsp], xmm1
___ir_block_16:                            ; offs: 0x4032ce
                movq xmm0, [rsp + 8]
                addsd xmm0, qword [rsp]
                add rsp, 16
                movq qword [rbp - 8 - 8], xmm0
___ir_block_17:                            ; offs: 0x4032eb
___ir_block_18:                            ; offs: 0x4032eb
___ir_block_19:                            ; offs: 0x4032eb
                movq xmm0, qword [rbp - 8 - 8]
                call doubleio_out
___ir_block_20:                            ; offs: 0x4032f8
___ir_block_21:                            ; offs: 0x4032f8
                movq xmm0, qword [rbp - 8 - 0]
                call doubleio_out
___ir_block_22:                            ; offs: 0x403305
___ir_block_23:                            ; offs: 0x403305
                mov rax, 0x0
                movq xmm0, rax
___ir_block_24:                            ; offs: 0x403314
                leave
                ret
___ir_block_25:                            ; offs: 0x403316
                leave
                pxor xmm0, xmm0
                ret
; ------------------------- Function definition end -----------------------



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
___ir_block_2:                            ; offs: 0x403231
                call ___func_4
                ; func call end

___ir_block_3:                            ; offs: 0x403236
                leave
                mov rax, 0x3c
                cvttsd2si rdi, xmm0
                syscall

___ir_block_4:                            ; offs: 0x403248
; =========================== Function definition =========================
___func_4:
                enter 0, 0
___ir_block_5:                            ; offs: 0x40324c
                call doubleio_in
                movq qword [rsp - 8 - 16 - 0], xmm0
___ir_block_6:                            ; offs: 0x40325a
___ir_block_7:                            ; offs: 0x40325a
                call ___func_14
                ; func call end

___ir_block_8:                            ; offs: 0x40325f
___ir_block_9:                            ; offs: 0x40325f
                movq xmm0, xmm0
                call doubleio_out
___ir_block_10:                            ; offs: 0x403268
___ir_block_11:                            ; offs: 0x403268
                mov rax, 0x0
                movq xmm0, rax
___ir_block_12:                            ; offs: 0x403277
                leave
                ret
___ir_block_13:                            ; offs: 0x403279
                leave
                pxor xmm0, xmm0
                ret
; ------------------------- Function definition end -----------------------


___ir_block_14:                            ; offs: 0x40327f
; =========================== Function definition =========================
___func_14:
                enter 8, 0
___ir_block_15:                            ; offs: 0x403283
___ir_block_16:                            ; offs: 0x403283
___ir_block_17:                            ; offs: 0x403283
                ; operands comparison: op1 > op2
                movq xmm1, qword [rbp - 8 - 0]
                mov rax, 0x3ff0000000000000
                movq xmm2, rax
                add rsp, -8
                movq xmm3, xmm1
                subsd xmm3, xmm2
                andpd xmm3, [DOUBLE_NEG_CONST]
                ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2)

                comisd xmm3, [EPSILON]
                ; fabs(op1 - op2) {'<' | '>'} EPSILON
                jc ___compare_17_false

                comisd xmm1, xmm2
                ; op1 {'<' | '>'} op2
                jc ___compare_17_false

                mov rax, 0x3ff0000000000000
                jmp ___compare_17_end

___compare_17_false:
                mov rax, 0x0
___compare_17_end:
                mov qword [rsp], rax

___ir_block_18:                            ; offs: 0x4032e8
                mov rax, 0x7fffffffffffffff
                and qword [rsp], rax
                movq xmm1, qword [rsp]
                add rsp, 8
                comisd xmm1, [EPSILON]
___ir_block_19:                            ; offs: 0x40330b
                jc ___ir_block_30
___ir_block_20:                            ; offs: 0x403311
                sub rsp, 8
                mov rax, qword [rbp - 8 - 0]

                mov qword [rsp], rax

___ir_block_21:                            ; offs: 0x403323
                sub rsp, 8
                mov rax, 0x3ff0000000000000
                mov qword [rsp], rax

___ir_block_22:                            ; offs: 0x403338
                movq xmm0, [rsp + 8]
                subsd xmm0, qword [rsp]
                add rsp, 16
                movq qword [rsp - 8 - 16 - 0], xmm0
___ir_block_23:                            ; offs: 0x403356
___ir_block_24:                            ; offs: 0x403356
                call ___func_14
                ; func call end

___ir_block_25:                            ; offs: 0x40335b
___ir_block_26:                            ; offs: 0x40335b
___ir_block_27:                            ; offs: 0x40335b
                mulsd xmm0, qword [rbp - 8 - 0]
___ir_block_28:                            ; offs: 0x403363
___ir_block_29:                            ; offs: 0x403363
                leave
                ret
___ir_block_30:                            ; offs: 0x403365
___ir_block_31:                            ; offs: 0x403365
___ir_block_32:                            ; offs: 0x403365
                mov rax, 0x3ff0000000000000
                movq xmm0, rax
___ir_block_33:                            ; offs: 0x403374
                leave
                ret
___ir_block_34:                            ; offs: 0x403376
                leave
                pxor xmm0, xmm0
                ret
; ------------------------- Function definition end -----------------------



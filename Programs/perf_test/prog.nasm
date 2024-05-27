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
                call ___func_25
                ; func call end

___ir_block_3:                            ; offs: 0x403239
                leave
                mov rax, 0x3c
                cvttsd2si rdi, xmm0
                syscall

___ir_block_4:                            ; offs: 0x40324b
; =========================== Function definition =========================
___func_4:
                enter 8, 0
___ir_block_5:                            ; offs: 0x40324f
___ir_block_6:                            ; offs: 0x40324f
___ir_block_7:                            ; offs: 0x40324f
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
                jc ___compare_7_false

                comisd xmm1, xmm2
                ; op1 {'<' | '>'} op2
                jc ___compare_7_false

                mov rax, 0x3ff0000000000000
                jmp ___compare_7_end

___compare_7_false:
                mov rax, 0x0
___compare_7_end:
                mov qword [rsp], rax

___ir_block_8:                            ; offs: 0x4032b4
                mov rax, 0x7fffffffffffffff
                and qword [rsp], rax
                movq xmm1, qword [rsp]
                add rsp, 8
                comisd xmm1, [EPSILON]
___ir_block_9:                            ; offs: 0x4032d7
                jc ___ir_block_20
___ir_block_10:                            ; offs: 0x4032dd
                sub rsp, 8
                mov rax, qword [rbp - 8 - 0]

                mov qword [rsp], rax

___ir_block_11:                            ; offs: 0x4032ef
                sub rsp, 8
                mov rax, 0x3ff0000000000000
                mov qword [rsp], rax

___ir_block_12:                            ; offs: 0x403304
                movq xmm0, [rsp + 8]
                subsd xmm0, qword [rsp]
                add rsp, 16
                movq qword [rsp - 8 - 16 - 0], xmm0
___ir_block_13:                            ; offs: 0x403322
___ir_block_14:                            ; offs: 0x403322
                call ___func_4
                ; func call end

___ir_block_15:                            ; offs: 0x403327
___ir_block_16:                            ; offs: 0x403327
___ir_block_17:                            ; offs: 0x403327
                mulsd xmm0, qword [rbp - 8 - 0]
___ir_block_18:                            ; offs: 0x40332f
___ir_block_19:                            ; offs: 0x40332f
                leave
                ret
___ir_block_20:                            ; offs: 0x403331
___ir_block_21:                            ; offs: 0x403331
___ir_block_22:                            ; offs: 0x403331
                mov rax, 0x3ff0000000000000
                movq xmm0, rax
___ir_block_23:                            ; offs: 0x403340
                leave
                ret
___ir_block_24:                            ; offs: 0x403342
                leave
                pxor xmm0, xmm0
                ret
; ------------------------- Function definition end -----------------------


___ir_block_25:                            ; offs: 0x403348
; =========================== Function definition =========================
___func_25:
                enter 8, 0
___ir_block_26:                            ; offs: 0x40334c
___ir_block_27:                            ; offs: 0x40334c
                mov rax, 0x416312d000000000
                mov qword [rbp - 8 - 0], rax

___ir_block_28:                            ; offs: 0x40335d
___ir_block_29:                            ; offs: 0x40335d
                sub rsp, 8
                mov rax, qword [rbp - 8 - 0]

                mov qword [rsp], rax

___ir_block_30:                            ; offs: 0x40336f
                sub rsp, 8
                mov rax, qword [rbp - 8 - 0]

                mov qword [rsp], rax

___ir_block_31:                            ; offs: 0x403381
                sub rsp, 8
                mov rax, 0x3ff0000000000000
                mov qword [rsp], rax

___ir_block_32:                            ; offs: 0x403396
                movq xmm0, [rsp + 8]
                subsd xmm0, qword [rsp]
                add rsp, 16
                movq qword [rbp - 8 - 0], xmm0
___ir_block_33:                            ; offs: 0x4033b3
___ir_block_34:                            ; offs: 0x4033b3
___ir_block_35:                            ; offs: 0x4033b3
                ; operands comparison: op1 > op2
                movq xmm1, qword [rsp]
                mov rax, 0x0
                movq xmm2, rax
                movq xmm3, xmm1
                subsd xmm3, xmm2
                andpd xmm3, [DOUBLE_NEG_CONST]
                ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2)

                comisd xmm3, [EPSILON]
                ; fabs(op1 - op2) {'<' | '>'} EPSILON
                jc ___compare_35_false

                comisd xmm1, xmm2
                ; op1 {'<' | '>'} op2
                jc ___compare_35_false

                mov rax, 0x3ff0000000000000
                jmp ___compare_35_end

___compare_35_false:
                mov rax, 0x0
___compare_35_end:
                mov qword [rsp], rax

___ir_block_36:                            ; offs: 0x40340e
                mov rax, 0x7fffffffffffffff
                and qword [rsp], rax
                movq xmm1, qword [rsp]
                add rsp, 8
                comisd xmm1, [EPSILON]
___ir_block_37:                            ; offs: 0x403431
                jc ___ir_block_42
___ir_block_38:                            ; offs: 0x403437
___ir_block_39:                            ; offs: 0x403437
                mov rax, 0x4034000000000000
                mov qword [rsp - 8 - 16 - 0], rax

___ir_block_40:                            ; offs: 0x403449
                call ___func_4
                ; func call end

___ir_block_41:                            ; offs: 0x40344e
                jmp ___ir_block_28
___ir_block_42:                            ; offs: 0x403453
___ir_block_43:                            ; offs: 0x403453
___ir_block_44:                            ; offs: 0x403453
                mov rax, 0x0
                movq xmm0, rax
___ir_block_45:                            ; offs: 0x403462
                leave
                ret
___ir_block_46:                            ; offs: 0x403464
                leave
                pxor xmm0, xmm0
                ret
; ------------------------- Function definition end -----------------------



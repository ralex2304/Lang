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
                enter 88, 0
___ir_block_5:                            ; offs: 0x40324c
                sub rsp, 8
                mov rax, 0x0
                mov qword [rsp], rax

___ir_block_6:                            ; offs: 0x403261
                lea rcx, qword [rbp - 8 - 0]
___ir_block_7:                            ; offs: 0x403268
                mov rax, [rsp]

                add rsp, 8
                mov qword [rcx], rax

___ir_block_8:                            ; offs: 0x403276
                sub rsp, 8
                mov rax, 0x3ff0000000000000
                mov qword [rsp], rax

___ir_block_9:                            ; offs: 0x40328b
                lea rcx, qword [rbp - 8 - 8]
___ir_block_10:                            ; offs: 0x403292
                mov rax, [rsp]

                add rsp, 8
                mov qword [rcx], rax

___ir_block_11:                            ; offs: 0x4032a0
                sub rsp, 8
                mov rax, 0x4000000000000000
                mov qword [rsp], rax

___ir_block_12:                            ; offs: 0x4032b5
                lea rcx, qword [rbp - 8 - 16]
___ir_block_13:                            ; offs: 0x4032bc
                mov rax, [rsp]

                add rsp, 8
                mov qword [rcx], rax

___ir_block_14:                            ; offs: 0x4032ca
                sub rsp, 8
                mov rax, 0x4008000000000000
                mov qword [rsp], rax

___ir_block_15:                            ; offs: 0x4032df
                lea rcx, qword [rbp - 8 - 24]
___ir_block_16:                            ; offs: 0x4032e6
                mov rax, [rsp]

                add rsp, 8
                mov qword [rcx], rax

___ir_block_17:                            ; offs: 0x4032f4
                sub rsp, 8
                mov rax, 0x4010000000000000
                mov qword [rsp], rax

___ir_block_18:                            ; offs: 0x403309
                lea rcx, qword [rbp - 8 - 32]
___ir_block_19:                            ; offs: 0x403310
                mov rax, [rsp]

                add rsp, 8
                mov qword [rcx], rax

___ir_block_20:                            ; offs: 0x40331e
                sub rsp, 8
                mov rax, 0x4014000000000000
                mov qword [rsp], rax

___ir_block_21:                            ; offs: 0x403333
                lea rcx, qword [rbp - 8 - 40]
___ir_block_22:                            ; offs: 0x40333a
                mov rax, [rsp]

                add rsp, 8
                mov qword [rcx], rax

___ir_block_23:                            ; offs: 0x403348
                sub rsp, 8
                mov rax, 0x4018000000000000
                mov qword [rsp], rax

___ir_block_24:                            ; offs: 0x40335d
                lea rcx, qword [rbp - 8 - 48]
___ir_block_25:                            ; offs: 0x403364
                mov rax, [rsp]

                add rsp, 8
                mov qword [rcx], rax

___ir_block_26:                            ; offs: 0x403372
___ir_block_27:                            ; offs: 0x403372
                mov rax, 0x0
                mov qword [rbp - 8 - 80], rax

___ir_block_28:                            ; offs: 0x403383
___ir_block_29:                            ; offs: 0x403383
                sub rsp, 8
                mov rax, qword [rbp - 8 - 80]

                mov qword [rsp], rax

___ir_block_30:                            ; offs: 0x403395
                sub rsp, 8
                mov rax, qword [rbp - 8 - 80]

                mov qword [rsp], rax

___ir_block_31:                            ; offs: 0x4033a7
                sub rsp, 8
                mov rax, 0x3ff0000000000000
                mov qword [rsp], rax

___ir_block_32:                            ; offs: 0x4033bc
                movq xmm0, [rsp + 8]
                addsd xmm0, qword [rsp]
                add rsp, 16
                movq qword [rbp - 8 - 80], xmm0
___ir_block_33:                            ; offs: 0x4033d9
___ir_block_34:                            ; offs: 0x4033d9
                lea rcx, qword [rbp - 8 - 0]
___ir_block_35:                            ; offs: 0x4033e0
                cvtsd2si rax, [rsp]
                add rsp, 8
                shl rax, 3
                sub rcx, rax
___ir_block_36:                            ; offs: 0x4033f4
___ir_block_37:                            ; offs: 0x4033f4
___ir_block_38:                            ; offs: 0x4033f4
                ; operands comparison: op1 < op2
                movq xmm1, qword [rcx]
                mov rax, 0x4014000000000000
                movq xmm2, rax
                add rsp, -8
                movq xmm3, xmm1
                subsd xmm3, xmm2
                andpd xmm3, [DOUBLE_NEG_CONST]
                ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2)

                comisd xmm3, [EPSILON]
                ; fabs(op1 - op2) {'<' | '>'} EPSILON
                jc ___compare_38_false

                comisd xmm1, xmm2
                ; op1 {'<' | '>'} op2
                jnc ___compare_38_false

                mov rax, 0x3ff0000000000000
                jmp ___compare_38_end

___compare_38_false:
                mov rax, 0x0
___compare_38_end:
                mov qword [rsp], rax

___ir_block_39:                            ; offs: 0x403455
                mov rax, 0x7fffffffffffffff
                and qword [rsp], rax
                movq xmm1, qword [rsp]
                add rsp, 8
                comisd xmm1, [EPSILON]
___ir_block_40:                            ; offs: 0x403478
                jc ___ir_block_47
___ir_block_41:                            ; offs: 0x40347e
                sub rsp, 8
                mov rax, qword [rbp - 8 - 80]

                mov qword [rsp], rax

___ir_block_42:                            ; offs: 0x403490
                lea rcx, qword [rbp - 8 - 0]
___ir_block_43:                            ; offs: 0x403497
                cvtsd2si rax, [rsp]
                add rsp, 8
                shl rax, 3
                sub rcx, rax
___ir_block_44:                            ; offs: 0x4034ab
___ir_block_45:                            ; offs: 0x4034ab
                movq xmm0, qword [rcx]
                call doubleio_out
___ir_block_46:                            ; offs: 0x4034b4
                jmp ___ir_block_28
___ir_block_47:                            ; offs: 0x4034b9
___ir_block_48:                            ; offs: 0x4034b9
                leave
                pxor xmm0, xmm0
                ret
; ------------------------- Function definition end -----------------------



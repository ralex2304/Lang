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
                enter 88, 0
___ir_block_5:                            ; offs: 0x40324f
                sub rsp, 8
                mov rax, 0x0
                mov qword [rsp], rax

___ir_block_6:                            ; offs: 0x403264
                lea rcx, qword [rbp - 8 - 0]
___ir_block_7:                            ; offs: 0x40326b
                mov rax, [rsp]

                add rsp, 8
                mov qword [rcx], rax

___ir_block_8:                            ; offs: 0x403279
                sub rsp, 8
                mov rax, 0x3ff0000000000000
                mov qword [rsp], rax

___ir_block_9:                            ; offs: 0x40328e
                lea rcx, qword [rbp - 8 - 8]
___ir_block_10:                            ; offs: 0x403295
                mov rax, [rsp]

                add rsp, 8
                mov qword [rcx], rax

___ir_block_11:                            ; offs: 0x4032a3
                sub rsp, 8
                mov rax, 0x4000000000000000
                mov qword [rsp], rax

___ir_block_12:                            ; offs: 0x4032b8
                lea rcx, qword [rbp - 8 - 16]
___ir_block_13:                            ; offs: 0x4032bf
                mov rax, [rsp]

                add rsp, 8
                mov qword [rcx], rax

___ir_block_14:                            ; offs: 0x4032cd
                sub rsp, 8
                mov rax, 0x4008000000000000
                mov qword [rsp], rax

___ir_block_15:                            ; offs: 0x4032e2
                lea rcx, qword [rbp - 8 - 24]
___ir_block_16:                            ; offs: 0x4032e9
                mov rax, [rsp]

                add rsp, 8
                mov qword [rcx], rax

___ir_block_17:                            ; offs: 0x4032f7
                sub rsp, 8
                mov rax, 0x4010000000000000
                mov qword [rsp], rax

___ir_block_18:                            ; offs: 0x40330c
                lea rcx, qword [rbp - 8 - 32]
___ir_block_19:                            ; offs: 0x403313
                mov rax, [rsp]

                add rsp, 8
                mov qword [rcx], rax

___ir_block_20:                            ; offs: 0x403321
                sub rsp, 8
                mov rax, 0x4014000000000000
                mov qword [rsp], rax

___ir_block_21:                            ; offs: 0x403336
                lea rcx, qword [rbp - 8 - 40]
___ir_block_22:                            ; offs: 0x40333d
                mov rax, [rsp]

                add rsp, 8
                mov qword [rcx], rax

___ir_block_23:                            ; offs: 0x40334b
                sub rsp, 8
                mov rax, 0x4018000000000000
                mov qword [rsp], rax

___ir_block_24:                            ; offs: 0x403360
                lea rcx, qword [rbp - 8 - 48]
___ir_block_25:                            ; offs: 0x403367
                mov rax, [rsp]

                add rsp, 8
                mov qword [rcx], rax

___ir_block_26:                            ; offs: 0x403375
___ir_block_27:                            ; offs: 0x403375
                mov rax, 0x0
                mov qword [rbp - 8 - 80], rax

___ir_block_28:                            ; offs: 0x403386
___ir_block_29:                            ; offs: 0x403386
                sub rsp, 8
                mov rax, qword [rbp - 8 - 80]

                mov qword [rsp], rax

___ir_block_30:                            ; offs: 0x403398
                sub rsp, 8
                mov rax, qword [rbp - 8 - 80]

                mov qword [rsp], rax

___ir_block_31:                            ; offs: 0x4033aa
                sub rsp, 8
                mov rax, 0x3ff0000000000000
                mov qword [rsp], rax

___ir_block_32:                            ; offs: 0x4033bf
                movq xmm0, [rsp + 8]
                addsd xmm0, qword [rsp]
                add rsp, 16
                movq qword [rbp - 8 - 80], xmm0
___ir_block_33:                            ; offs: 0x4033dc
___ir_block_34:                            ; offs: 0x4033dc
                lea rcx, qword [rbp - 8 - 0]
___ir_block_35:                            ; offs: 0x4033e3
                cvtsd2si rax, [rsp]
                add rsp, 8
                shl rax, 3
                sub rcx, rax
___ir_block_36:                            ; offs: 0x4033f7
___ir_block_37:                            ; offs: 0x4033f7
___ir_block_38:                            ; offs: 0x4033f7
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

___ir_block_39:                            ; offs: 0x403458
                mov rax, 0x7fffffffffffffff
                and qword [rsp], rax
                movq xmm1, qword [rsp]
                add rsp, 8
                comisd xmm1, [EPSILON]
___ir_block_40:                            ; offs: 0x40347b
                jc ___ir_block_47
___ir_block_41:                            ; offs: 0x403481
                sub rsp, 8
                mov rax, qword [rbp - 8 - 80]

                mov qword [rsp], rax

___ir_block_42:                            ; offs: 0x403493
                lea rcx, qword [rbp - 8 - 0]
___ir_block_43:                            ; offs: 0x40349a
                cvtsd2si rax, [rsp]
                add rsp, 8
                shl rax, 3
                sub rcx, rax
___ir_block_44:                            ; offs: 0x4034ae
___ir_block_45:                            ; offs: 0x4034ae
                movq xmm0, qword [rcx]
                call doubleio_out
___ir_block_46:                            ; offs: 0x4034b7
                jmp ___ir_block_28
___ir_block_47:                            ; offs: 0x4034bc
___ir_block_48:                            ; offs: 0x4034bc
                leave
                pxor xmm0, xmm0
                ret
; ------------------------- Function definition end -----------------------



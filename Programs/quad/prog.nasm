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
                enter 24, 0
___ir_block_5:                            ; offs: 0x40324f
                call doubleio_in
                movq qword [rbp - 8 - 0], xmm0
___ir_block_6:                            ; offs: 0x40325c
___ir_block_7:                            ; offs: 0x40325c
                call doubleio_in
                movq qword [rbp - 8 - 8], xmm0
___ir_block_8:                            ; offs: 0x403269
___ir_block_9:                            ; offs: 0x403269
                call doubleio_in
                movq qword [rbp - 8 - 16], xmm0
___ir_block_10:                            ; offs: 0x403276
___ir_block_11:                            ; offs: 0x403276
___ir_block_12:                            ; offs: 0x403276
                mov rax, qword [rbp - 8 - 0]

                mov qword [rsp - 8 - 16 - 0], rax

___ir_block_13:                            ; offs: 0x403285
___ir_block_14:                            ; offs: 0x403285
                mov rax, qword [rbp - 8 - 8]

                mov qword [rsp - 8 - 16 - 8], rax

___ir_block_15:                            ; offs: 0x403294
___ir_block_16:                            ; offs: 0x403294
                mov rax, qword [rbp - 8 - 16]

                mov qword [rsp - 8 - 16 - 16], rax

___ir_block_17:                            ; offs: 0x4032a3
                call ___func_22
                ; func call end

___ir_block_18:                            ; offs: 0x4032a8
___ir_block_19:                            ; offs: 0x4032a8
___ir_block_20:                            ; offs: 0x4032a8
                leave
                ret
___ir_block_21:                            ; offs: 0x4032aa
                leave
                pxor xmm0, xmm0
                ret
; ------------------------- Function definition end -----------------------


___ir_block_22:                            ; offs: 0x4032b0
; =========================== Function definition =========================
___func_22:
                enter 32, 0
___ir_block_23:                            ; offs: 0x4032b4
___ir_block_24:                            ; offs: 0x4032b4
___ir_block_25:                            ; offs: 0x4032b4
                ; operands comparison: op1 == op2
                movq xmm1, qword [rbp - 8 - 0]
                mov rax, 0x0
                movq xmm2, rax
                add rsp, -8
                movq xmm3, xmm1
                subsd xmm3, xmm2
                andpd xmm3, [DOUBLE_NEG_CONST]
                ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2)

                comisd xmm3, [EPSILON]
                ; fabs(op1 - op2) {'<' | '>'} EPSILON
                jnc ___compare_25_false

                mov rax, 0x3ff0000000000000
                jmp ___compare_25_end

___compare_25_false:
                mov rax, 0x0
___compare_25_end:
                mov qword [rsp], rax

___ir_block_26:                            ; offs: 0x40330f
                mov rax, 0x7fffffffffffffff
                and qword [rsp], rax
                movq xmm1, qword [rsp]
                add rsp, 8
                comisd xmm1, [EPSILON]
___ir_block_27:                            ; offs: 0x403332
                jc ___ir_block_36
___ir_block_28:                            ; offs: 0x403338
___ir_block_29:                            ; offs: 0x403338
                mov rax, qword [rbp - 8 - 8]

                mov qword [rsp - 8 - 16 - 0], rax

___ir_block_30:                            ; offs: 0x403347
___ir_block_31:                            ; offs: 0x403347
                mov rax, qword [rbp - 8 - 16]

                mov qword [rsp - 8 - 16 - 8], rax

___ir_block_32:                            ; offs: 0x403356
                call ___func_103
                ; func call end

___ir_block_33:                            ; offs: 0x40335b
___ir_block_34:                            ; offs: 0x40335b
___ir_block_35:                            ; offs: 0x40335b
                leave
                ret
___ir_block_36:                            ; offs: 0x40335d
___ir_block_37:                            ; offs: 0x40335d
___ir_block_38:                            ; offs: 0x40335d
___ir_block_39:                            ; offs: 0x40335d
                sub rsp, 8
                movq xmm0, qword [rbp - 8 - 8]
                mulsd xmm0, qword [rbp - 8 - 8]
                movq qword [rsp], xmm0
___ir_block_40:                            ; offs: 0x403379
                sub rsp, 8
                mov rax, 0x4010000000000000
                mov qword [rsp], rax

___ir_block_41:                            ; offs: 0x40338e
___ir_block_42:                            ; offs: 0x40338e
___ir_block_43:                            ; offs: 0x40338e
                sub rsp, 8
                movq xmm0, qword [rbp - 8 - 0]
                mulsd xmm0, qword [rbp - 8 - 16]
                movq qword [rsp], xmm0
___ir_block_44:                            ; offs: 0x4033aa
                movq xmm0, [rsp + 8]
                mulsd xmm0, qword [rsp]
                add rsp, 8
                movq qword [rsp], xmm0
___ir_block_45:                            ; offs: 0x4033c4
                movq xmm0, [rsp + 8]
                subsd xmm0, qword [rsp]
                add rsp, 16
                movq qword [rbp - 8 - 24], xmm0
___ir_block_46:                            ; offs: 0x4033e1
___ir_block_47:                            ; offs: 0x4033e1
___ir_block_48:                            ; offs: 0x4033e1
___ir_block_49:                            ; offs: 0x4033e1
                ; operands comparison: op1 < op2
                movq xmm1, qword [rbp - 8 - 24]
                mov rax, 0x0
                movq xmm2, rax
                add rsp, -8
                movq xmm3, xmm1
                subsd xmm3, xmm2
                andpd xmm3, [DOUBLE_NEG_CONST]
                ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2)

                comisd xmm3, [EPSILON]
                ; fabs(op1 - op2) {'<' | '>'} EPSILON
                jc ___compare_49_false

                comisd xmm1, xmm2
                ; op1 {'<' | '>'} op2
                jnc ___compare_49_false

                mov rax, 0x3ff0000000000000
                jmp ___compare_49_end

___compare_49_false:
                mov rax, 0x0
___compare_49_end:
                mov qword [rsp], rax

___ir_block_50:                            ; offs: 0x403446
                mov rax, 0x7fffffffffffffff
                and qword [rsp], rax
                movq xmm1, qword [rsp]
                add rsp, 8
                comisd xmm1, [EPSILON]
___ir_block_51:                            ; offs: 0x403469
                jc ___ir_block_57
___ir_block_52:                            ; offs: 0x40346f
___ir_block_53:                            ; offs: 0x40346f
                mov rax, 0x0
                movq xmm0, rax
                call doubleio_out
___ir_block_54:                            ; offs: 0x403483
___ir_block_55:                            ; offs: 0x403483
                mov rax, 0x0
                movq xmm0, rax
___ir_block_56:                            ; offs: 0x403492
                leave
                ret
___ir_block_57:                            ; offs: 0x403494
___ir_block_58:                            ; offs: 0x403494
___ir_block_59:                            ; offs: 0x403494
___ir_block_60:                            ; offs: 0x403494
                ; operands comparison: op1 == op2
                movq xmm1, qword [rbp - 8 - 24]
                mov rax, 0x0
                movq xmm2, rax
                add rsp, -8
                movq xmm3, xmm1
                subsd xmm3, xmm2
                andpd xmm3, [DOUBLE_NEG_CONST]
                ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2)

                comisd xmm3, [EPSILON]
                ; fabs(op1 - op2) {'<' | '>'} EPSILON
                jnc ___compare_60_false

                mov rax, 0x3ff0000000000000
                jmp ___compare_60_end

___compare_60_false:
                mov rax, 0x0
___compare_60_end:
                mov qword [rsp], rax

___ir_block_61:                            ; offs: 0x4034ef
                mov rax, 0x7fffffffffffffff
                and qword [rsp], rax
                movq xmm1, qword [rsp]
                add rsp, 8
                comisd xmm1, [EPSILON]
___ir_block_62:                            ; offs: 0x403512
                jc ___ir_block_75
___ir_block_63:                            ; offs: 0x403518
___ir_block_64:                            ; offs: 0x403518
                mov rax, 0x3ff0000000000000
                movq xmm0, rax
                call doubleio_out
___ir_block_65:                            ; offs: 0x40352c
___ir_block_66:                            ; offs: 0x40352c
                mov rax, 0x8000000000000000
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 8]

                mov qword [rsp], rdx

                xor qword [rsp], rax
___ir_block_67:                            ; offs: 0x40354c
                sub rsp, 8
                mov rax, 0x4000000000000000
                mov qword [rsp], rax

___ir_block_68:                            ; offs: 0x403561
___ir_block_69:                            ; offs: 0x403561
                movq xmm0, qword [rsp]
                mulsd xmm0, qword [rbp - 8 - 0]
                movq qword [rsp], xmm0
___ir_block_70:                            ; offs: 0x403573
                movq xmm0, [rsp + 8]
                divsd xmm0, qword [rsp]
                add rsp, 8
                movq qword [rsp], xmm0
___ir_block_71:                            ; offs: 0x40358d
                movq xmm0, qword [rsp]
                add rsp, 8
                call doubleio_out
___ir_block_72:                            ; offs: 0x40359e
___ir_block_73:                            ; offs: 0x40359e
                mov rax, 0x0
                movq xmm0, rax
___ir_block_74:                            ; offs: 0x4035ad
                leave
                ret
___ir_block_75:                            ; offs: 0x4035af
___ir_block_76:                            ; offs: 0x4035af
___ir_block_77:                            ; offs: 0x4035af
                sqrtsd xmm0, qword [rbp - 8 - 24]
                movq qword [rbp - 8 - 24], xmm0
___ir_block_78:                            ; offs: 0x4035bf
___ir_block_79:                            ; offs: 0x4035bf
___ir_block_80:                            ; offs: 0x4035bf
                mov rax, 0x4000000000000000
                movq xmm0, rax
                call doubleio_out
___ir_block_81:                            ; offs: 0x4035d3
___ir_block_82:                            ; offs: 0x4035d3
                mov rax, 0x8000000000000000
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 8]

                mov qword [rsp], rdx

                xor qword [rsp], rax
___ir_block_83:                            ; offs: 0x4035f3
___ir_block_84:                            ; offs: 0x4035f3
                movq xmm0, qword [rsp]
                subsd xmm0, qword [rbp - 8 - 24]
                movq qword [rsp], xmm0
___ir_block_85:                            ; offs: 0x403605
                sub rsp, 8
                mov rax, 0x4000000000000000
                mov qword [rsp], rax

___ir_block_86:                            ; offs: 0x40361a
___ir_block_87:                            ; offs: 0x40361a
                movq xmm0, qword [rsp]
                mulsd xmm0, qword [rbp - 8 - 0]
                movq qword [rsp], xmm0
___ir_block_88:                            ; offs: 0x40362c
                movq xmm0, [rsp + 8]
                divsd xmm0, qword [rsp]
                add rsp, 8
                movq qword [rsp], xmm0
___ir_block_89:                            ; offs: 0x403646
                movq xmm0, qword [rsp]
                add rsp, 8
                call doubleio_out
___ir_block_90:                            ; offs: 0x403657
___ir_block_91:                            ; offs: 0x403657
                mov rax, 0x8000000000000000
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 8]

                mov qword [rsp], rdx

                xor qword [rsp], rax
___ir_block_92:                            ; offs: 0x403677
___ir_block_93:                            ; offs: 0x403677
                movq xmm0, qword [rsp]
                addsd xmm0, qword [rbp - 8 - 24]
                movq qword [rsp], xmm0
___ir_block_94:                            ; offs: 0x403689
                sub rsp, 8
                mov rax, 0x4000000000000000
                mov qword [rsp], rax

___ir_block_95:                            ; offs: 0x40369e
___ir_block_96:                            ; offs: 0x40369e
                movq xmm0, qword [rsp]
                mulsd xmm0, qword [rbp - 8 - 0]
                movq qword [rsp], xmm0
___ir_block_97:                            ; offs: 0x4036b0
                movq xmm0, [rsp + 8]
                divsd xmm0, qword [rsp]
                add rsp, 8
                movq qword [rsp], xmm0
___ir_block_98:                            ; offs: 0x4036ca
                movq xmm0, qword [rsp]
                add rsp, 8
                call doubleio_out
___ir_block_99:                            ; offs: 0x4036db
___ir_block_100:                            ; offs: 0x4036db
                mov rax, 0x0
                movq xmm0, rax
___ir_block_101:                            ; offs: 0x4036ea
                leave
                ret
___ir_block_102:                            ; offs: 0x4036ec
                leave
                pxor xmm0, xmm0
                ret
; ------------------------- Function definition end -----------------------


___ir_block_103:                            ; offs: 0x4036f2
; =========================== Function definition =========================
___func_103:
                enter 16, 0
___ir_block_104:                            ; offs: 0x4036f6
___ir_block_105:                            ; offs: 0x4036f6
___ir_block_106:                            ; offs: 0x4036f6
                ; operands comparison: op1 == op2
                movq xmm1, qword [rbp - 8 - 0]
                mov rax, 0x0
                movq xmm2, rax
                add rsp, -8
                movq xmm3, xmm1
                subsd xmm3, xmm2
                andpd xmm3, [DOUBLE_NEG_CONST]
                ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2)

                comisd xmm3, [EPSILON]
                ; fabs(op1 - op2) {'<' | '>'} EPSILON
                jnc ___compare_106_false

                mov rax, 0x3ff0000000000000
                jmp ___compare_106_end

___compare_106_false:
                mov rax, 0x0
___compare_106_end:
                mov qword [rsp], rax

___ir_block_107:                            ; offs: 0x403751
                mov rax, 0x7fffffffffffffff
                and qword [rsp], rax
                movq xmm1, qword [rsp]
                add rsp, 8
                comisd xmm1, [EPSILON]
___ir_block_108:                            ; offs: 0x403774
                jc ___ir_block_125
___ir_block_109:                            ; offs: 0x40377a
___ir_block_110:                            ; offs: 0x40377a
___ir_block_111:                            ; offs: 0x40377a
                ; operands comparison: op1 == op2
                movq xmm1, qword [rbp - 8 - 8]
                mov rax, 0x0
                movq xmm2, rax
                add rsp, -8
                movq xmm3, xmm1
                subsd xmm3, xmm2
                andpd xmm3, [DOUBLE_NEG_CONST]
                ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2)

                comisd xmm3, [EPSILON]
                ; fabs(op1 - op2) {'<' | '>'} EPSILON
                jnc ___compare_111_false

                mov rax, 0x3ff0000000000000
                jmp ___compare_111_end

___compare_111_false:
                mov rax, 0x0
___compare_111_end:
                mov qword [rsp], rax

___ir_block_112:                            ; offs: 0x4037d5
                mov rax, 0x7fffffffffffffff
                and qword [rsp], rax
                movq xmm1, qword [rsp]
                add rsp, 8
                comisd xmm1, [EPSILON]
___ir_block_113:                            ; offs: 0x4037f8
                jc ___ir_block_119
___ir_block_114:                            ; offs: 0x4037fe
___ir_block_115:                            ; offs: 0x4037fe
                mov rax, 0xbff0000000000000
                movq xmm0, rax
                call doubleio_out
___ir_block_116:                            ; offs: 0x403812
___ir_block_117:                            ; offs: 0x403812
                mov rax, 0x0
                movq xmm0, rax
___ir_block_118:                            ; offs: 0x403821
                leave
                ret
___ir_block_119:                            ; offs: 0x403823
___ir_block_120:                            ; offs: 0x403823
___ir_block_121:                            ; offs: 0x403823
                mov rax, 0x0
                movq xmm0, rax
                call doubleio_out
___ir_block_122:                            ; offs: 0x403837
___ir_block_123:                            ; offs: 0x403837
                mov rax, 0x0
                movq xmm0, rax
___ir_block_124:                            ; offs: 0x403846
                leave
                ret
___ir_block_125:                            ; offs: 0x403848
___ir_block_126:                            ; offs: 0x403848
___ir_block_127:                            ; offs: 0x403848
                mov rax, 0x3ff0000000000000
                movq xmm0, rax
                call doubleio_out
___ir_block_128:                            ; offs: 0x40385c
___ir_block_129:                            ; offs: 0x40385c
                mov rax, 0x8000000000000000
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 8]

                mov qword [rsp], rdx

                xor qword [rsp], rax
___ir_block_130:                            ; offs: 0x40387c
___ir_block_131:                            ; offs: 0x40387c
                movq xmm0, qword [rsp]
                divsd xmm0, qword [rbp - 8 - 0]
                movq qword [rsp], xmm0
___ir_block_132:                            ; offs: 0x40388e
                movq xmm0, qword [rsp]
                add rsp, 8
                call doubleio_out
___ir_block_133:                            ; offs: 0x40389f
___ir_block_134:                            ; offs: 0x40389f
                mov rax, 0x0
                movq xmm0, rax
___ir_block_135:                            ; offs: 0x4038ae
                leave
                ret
___ir_block_136:                            ; offs: 0x4038b0
                leave
                pxor xmm0, xmm0
                ret
; ------------------------- Function definition end -----------------------



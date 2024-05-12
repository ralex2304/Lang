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
                enter 24, 0
___ir_block_5:                            ; offs: 0x40324c
                call doubleio_in
                movq qword [rbp - 8 - 0], xmm0
___ir_block_6:                            ; offs: 0x403259
___ir_block_7:                            ; offs: 0x403259
                call doubleio_in
                movq qword [rbp - 8 - 8], xmm0
___ir_block_8:                            ; offs: 0x403266
___ir_block_9:                            ; offs: 0x403266
                call doubleio_in
                movq qword [rbp - 8 - 16], xmm0
___ir_block_10:                            ; offs: 0x403273
___ir_block_11:                            ; offs: 0x403273
___ir_block_12:                            ; offs: 0x403273
                mov rdx, qword [rbp - 8 - 0]

                mov qword [rsp - 8 - 16 - 0], rdx

___ir_block_13:                            ; offs: 0x403282
___ir_block_14:                            ; offs: 0x403282
                mov rdx, qword [rbp - 8 - 8]

                mov qword [rsp - 8 - 16 - 8], rdx

___ir_block_15:                            ; offs: 0x403291
___ir_block_16:                            ; offs: 0x403291
                mov rdx, qword [rbp - 8 - 16]

                mov qword [rsp - 8 - 16 - 16], rdx

___ir_block_17:                            ; offs: 0x4032a0
                call ___func_22
                ; func call end

___ir_block_18:                            ; offs: 0x4032a5
___ir_block_19:                            ; offs: 0x4032a5
___ir_block_20:                            ; offs: 0x4032a5
                leave
                ret
___ir_block_21:                            ; offs: 0x4032a7
                leave
                pxor xmm0, xmm0
                ret
; ------------------------- Function definition end -----------------------


___ir_block_22:                            ; offs: 0x4032ad
; =========================== Function definition =========================
___func_22:
                enter 32, 0
___ir_block_23:                            ; offs: 0x4032b1
___ir_block_24:                            ; offs: 0x4032b1
___ir_block_25:                            ; offs: 0x4032b1
                ; operands comparison: op1 == op2
                movq xmm1, qword [rbp - 8 - 0]
                mov rdx, 0x0
                movq xmm2, rdx
                add rsp, -8
                movq xmm3, xmm1
                subsd xmm3, xmm2
                andpd xmm3, [DOUBLE_NEG_CONST]
                ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2)

                comisd xmm3, [EPSILON]
                ; fabs(op1 - op2) {'<' | '>'} EPSILON
                jnc ___compare_25_false

                mov rdx, 0x3ff0000000000000
                jmp ___compare_25_end

___compare_25_false:
                mov rdx, 0x0
___compare_25_end:
                mov qword [rsp], rdx

___ir_block_26:                            ; offs: 0x40330c
                mov rdx, 0x7fffffffffffffff
                and qword [rsp], rdx
                movq xmm1, qword [rsp]
                add rsp, 8
                comisd xmm1, [EPSILON]
___ir_block_27:                            ; offs: 0x40332f
                jc ___ir_block_36
___ir_block_28:                            ; offs: 0x403335
___ir_block_29:                            ; offs: 0x403335
                mov rdx, qword [rbp - 8 - 8]

                mov qword [rsp - 8 - 16 - 0], rdx

___ir_block_30:                            ; offs: 0x403344
___ir_block_31:                            ; offs: 0x403344
                mov rdx, qword [rbp - 8 - 16]

                mov qword [rsp - 8 - 16 - 8], rdx

___ir_block_32:                            ; offs: 0x403353
                call ___func_103
                ; func call end

___ir_block_33:                            ; offs: 0x403358
___ir_block_34:                            ; offs: 0x403358
___ir_block_35:                            ; offs: 0x403358
                leave
                ret
___ir_block_36:                            ; offs: 0x40335a
___ir_block_37:                            ; offs: 0x40335a
___ir_block_38:                            ; offs: 0x40335a
___ir_block_39:                            ; offs: 0x40335a
                sub rsp, 8
                movq xmm0, qword [rbp - 8 - 8]
                mulsd xmm0, qword [rbp - 8 - 8]
                movq qword [rsp], xmm0
___ir_block_40:                            ; offs: 0x403376
                sub rsp, 8
                mov rdx, 0x4010000000000000
                mov qword [rsp], rdx

___ir_block_41:                            ; offs: 0x40338b
___ir_block_42:                            ; offs: 0x40338b
___ir_block_43:                            ; offs: 0x40338b
                sub rsp, 8
                movq xmm0, qword [rbp - 8 - 0]
                mulsd xmm0, qword [rbp - 8 - 16]
                movq qword [rsp], xmm0
___ir_block_44:                            ; offs: 0x4033a7
                movq xmm0, [rsp + 8]
                mulsd xmm0, qword [rsp]
                add rsp, 8
                movq qword [rsp], xmm0
___ir_block_45:                            ; offs: 0x4033c1
                movq xmm0, [rsp + 8]
                subsd xmm0, qword [rsp]
                add rsp, 16
                movq qword [rbp - 8 - 24], xmm0
___ir_block_46:                            ; offs: 0x4033de
___ir_block_47:                            ; offs: 0x4033de
___ir_block_48:                            ; offs: 0x4033de
___ir_block_49:                            ; offs: 0x4033de
                ; operands comparison: op1 < op2
                movq xmm1, qword [rbp - 8 - 24]
                mov rdx, 0x0
                movq xmm2, rdx
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

                mov rdx, 0x3ff0000000000000
                jmp ___compare_49_end

___compare_49_false:
                mov rdx, 0x0
___compare_49_end:
                mov qword [rsp], rdx

___ir_block_50:                            ; offs: 0x403443
                mov rdx, 0x7fffffffffffffff
                and qword [rsp], rdx
                movq xmm1, qword [rsp]
                add rsp, 8
                comisd xmm1, [EPSILON]
___ir_block_51:                            ; offs: 0x403466
                jc ___ir_block_57
___ir_block_52:                            ; offs: 0x40346c
___ir_block_53:                            ; offs: 0x40346c
                mov rdx, 0x0
                movq xmm0, rdx
                call doubleio_out
___ir_block_54:                            ; offs: 0x403480
___ir_block_55:                            ; offs: 0x403480
                mov rdx, 0x0
                movq xmm0, rdx
___ir_block_56:                            ; offs: 0x40348f
                leave
                ret
___ir_block_57:                            ; offs: 0x403491
___ir_block_58:                            ; offs: 0x403491
___ir_block_59:                            ; offs: 0x403491
___ir_block_60:                            ; offs: 0x403491
                ; operands comparison: op1 == op2
                movq xmm1, qword [rbp - 8 - 24]
                mov rdx, 0x0
                movq xmm2, rdx
                add rsp, -8
                movq xmm3, xmm1
                subsd xmm3, xmm2
                andpd xmm3, [DOUBLE_NEG_CONST]
                ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2)

                comisd xmm3, [EPSILON]
                ; fabs(op1 - op2) {'<' | '>'} EPSILON
                jnc ___compare_60_false

                mov rdx, 0x3ff0000000000000
                jmp ___compare_60_end

___compare_60_false:
                mov rdx, 0x0
___compare_60_end:
                mov qword [rsp], rdx

___ir_block_61:                            ; offs: 0x4034ec
                mov rdx, 0x7fffffffffffffff
                and qword [rsp], rdx
                movq xmm1, qword [rsp]
                add rsp, 8
                comisd xmm1, [EPSILON]
___ir_block_62:                            ; offs: 0x40350f
                jc ___ir_block_75
___ir_block_63:                            ; offs: 0x403515
___ir_block_64:                            ; offs: 0x403515
                mov rdx, 0x3ff0000000000000
                movq xmm0, rdx
                call doubleio_out
___ir_block_65:                            ; offs: 0x403529
___ir_block_66:                            ; offs: 0x403529
                mov rdx, 0x8000000000000000
                sub rsp, 8
                mov rax, qword [rbp - 8 - 8]

                mov qword [rsp], rax

                xor qword [rsp], rdx
___ir_block_67:                            ; offs: 0x403549
                sub rsp, 8
                mov rdx, 0x4000000000000000
                mov qword [rsp], rdx

___ir_block_68:                            ; offs: 0x40355e
___ir_block_69:                            ; offs: 0x40355e
                movq xmm0, qword [rsp]
                mulsd xmm0, qword [rbp - 8 - 0]
                movq qword [rsp], xmm0
___ir_block_70:                            ; offs: 0x403570
                movq xmm0, [rsp + 8]
                divsd xmm0, qword [rsp]
                add rsp, 8
                movq qword [rsp], xmm0
___ir_block_71:                            ; offs: 0x40358a
                movq xmm0, qword [rsp]
                add rsp, 8
                call doubleio_out
___ir_block_72:                            ; offs: 0x40359b
___ir_block_73:                            ; offs: 0x40359b
                mov rdx, 0x0
                movq xmm0, rdx
___ir_block_74:                            ; offs: 0x4035aa
                leave
                ret
___ir_block_75:                            ; offs: 0x4035ac
___ir_block_76:                            ; offs: 0x4035ac
___ir_block_77:                            ; offs: 0x4035ac
                sqrtsd xmm0, qword [rbp - 8 - 24]
                movq qword [rbp - 8 - 24], xmm0
___ir_block_78:                            ; offs: 0x4035bc
___ir_block_79:                            ; offs: 0x4035bc
___ir_block_80:                            ; offs: 0x4035bc
                mov rdx, 0x4000000000000000
                movq xmm0, rdx
                call doubleio_out
___ir_block_81:                            ; offs: 0x4035d0
___ir_block_82:                            ; offs: 0x4035d0
                mov rdx, 0x8000000000000000
                sub rsp, 8
                mov rax, qword [rbp - 8 - 8]

                mov qword [rsp], rax

                xor qword [rsp], rdx
___ir_block_83:                            ; offs: 0x4035f0
___ir_block_84:                            ; offs: 0x4035f0
                movq xmm0, qword [rsp]
                subsd xmm0, qword [rbp - 8 - 24]
                movq qword [rsp], xmm0
___ir_block_85:                            ; offs: 0x403602
                sub rsp, 8
                mov rdx, 0x4000000000000000
                mov qword [rsp], rdx

___ir_block_86:                            ; offs: 0x403617
___ir_block_87:                            ; offs: 0x403617
                movq xmm0, qword [rsp]
                mulsd xmm0, qword [rbp - 8 - 0]
                movq qword [rsp], xmm0
___ir_block_88:                            ; offs: 0x403629
                movq xmm0, [rsp + 8]
                divsd xmm0, qword [rsp]
                add rsp, 8
                movq qword [rsp], xmm0
___ir_block_89:                            ; offs: 0x403643
                movq xmm0, qword [rsp]
                add rsp, 8
                call doubleio_out
___ir_block_90:                            ; offs: 0x403654
___ir_block_91:                            ; offs: 0x403654
                mov rdx, 0x8000000000000000
                sub rsp, 8
                mov rax, qword [rbp - 8 - 8]

                mov qword [rsp], rax

                xor qword [rsp], rdx
___ir_block_92:                            ; offs: 0x403674
___ir_block_93:                            ; offs: 0x403674
                movq xmm0, qword [rsp]
                addsd xmm0, qword [rbp - 8 - 24]
                movq qword [rsp], xmm0
___ir_block_94:                            ; offs: 0x403686
                sub rsp, 8
                mov rdx, 0x4000000000000000
                mov qword [rsp], rdx

___ir_block_95:                            ; offs: 0x40369b
___ir_block_96:                            ; offs: 0x40369b
                movq xmm0, qword [rsp]
                mulsd xmm0, qword [rbp - 8 - 0]
                movq qword [rsp], xmm0
___ir_block_97:                            ; offs: 0x4036ad
                movq xmm0, [rsp + 8]
                divsd xmm0, qword [rsp]
                add rsp, 8
                movq qword [rsp], xmm0
___ir_block_98:                            ; offs: 0x4036c7
                movq xmm0, qword [rsp]
                add rsp, 8
                call doubleio_out
___ir_block_99:                            ; offs: 0x4036d8
___ir_block_100:                            ; offs: 0x4036d8
                mov rdx, 0x0
                movq xmm0, rdx
___ir_block_101:                            ; offs: 0x4036e7
                leave
                ret
___ir_block_102:                            ; offs: 0x4036e9
                leave
                pxor xmm0, xmm0
                ret
; ------------------------- Function definition end -----------------------


___ir_block_103:                            ; offs: 0x4036ef
; =========================== Function definition =========================
___func_103:
                enter 16, 0
___ir_block_104:                            ; offs: 0x4036f3
___ir_block_105:                            ; offs: 0x4036f3
___ir_block_106:                            ; offs: 0x4036f3
                ; operands comparison: op1 == op2
                movq xmm1, qword [rbp - 8 - 0]
                mov rdx, 0x0
                movq xmm2, rdx
                add rsp, -8
                movq xmm3, xmm1
                subsd xmm3, xmm2
                andpd xmm3, [DOUBLE_NEG_CONST]
                ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2)

                comisd xmm3, [EPSILON]
                ; fabs(op1 - op2) {'<' | '>'} EPSILON
                jnc ___compare_106_false

                mov rdx, 0x3ff0000000000000
                jmp ___compare_106_end

___compare_106_false:
                mov rdx, 0x0
___compare_106_end:
                mov qword [rsp], rdx

___ir_block_107:                            ; offs: 0x40374e
                mov rdx, 0x7fffffffffffffff
                and qword [rsp], rdx
                movq xmm1, qword [rsp]
                add rsp, 8
                comisd xmm1, [EPSILON]
___ir_block_108:                            ; offs: 0x403771
                jc ___ir_block_125
___ir_block_109:                            ; offs: 0x403777
___ir_block_110:                            ; offs: 0x403777
___ir_block_111:                            ; offs: 0x403777
                ; operands comparison: op1 == op2
                movq xmm1, qword [rbp - 8 - 8]
                mov rdx, 0x0
                movq xmm2, rdx
                add rsp, -8
                movq xmm3, xmm1
                subsd xmm3, xmm2
                andpd xmm3, [DOUBLE_NEG_CONST]
                ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2)

                comisd xmm3, [EPSILON]
                ; fabs(op1 - op2) {'<' | '>'} EPSILON
                jnc ___compare_111_false

                mov rdx, 0x3ff0000000000000
                jmp ___compare_111_end

___compare_111_false:
                mov rdx, 0x0
___compare_111_end:
                mov qword [rsp], rdx

___ir_block_112:                            ; offs: 0x4037d2
                mov rdx, 0x7fffffffffffffff
                and qword [rsp], rdx
                movq xmm1, qword [rsp]
                add rsp, 8
                comisd xmm1, [EPSILON]
___ir_block_113:                            ; offs: 0x4037f5
                jc ___ir_block_119
___ir_block_114:                            ; offs: 0x4037fb
___ir_block_115:                            ; offs: 0x4037fb
                mov rdx, 0xbff0000000000000
                movq xmm0, rdx
                call doubleio_out
___ir_block_116:                            ; offs: 0x40380f
___ir_block_117:                            ; offs: 0x40380f
                mov rdx, 0x0
                movq xmm0, rdx
___ir_block_118:                            ; offs: 0x40381e
                leave
                ret
___ir_block_119:                            ; offs: 0x403820
___ir_block_120:                            ; offs: 0x403820
___ir_block_121:                            ; offs: 0x403820
                mov rdx, 0x0
                movq xmm0, rdx
                call doubleio_out
___ir_block_122:                            ; offs: 0x403834
___ir_block_123:                            ; offs: 0x403834
                mov rdx, 0x0
                movq xmm0, rdx
___ir_block_124:                            ; offs: 0x403843
                leave
                ret
___ir_block_125:                            ; offs: 0x403845
___ir_block_126:                            ; offs: 0x403845
___ir_block_127:                            ; offs: 0x403845
                mov rdx, 0x3ff0000000000000
                movq xmm0, rdx
                call doubleio_out
___ir_block_128:                            ; offs: 0x403859
___ir_block_129:                            ; offs: 0x403859
                mov rdx, 0x8000000000000000
                sub rsp, 8
                mov rax, qword [rbp - 8 - 8]

                mov qword [rsp], rax

                xor qword [rsp], rdx
___ir_block_130:                            ; offs: 0x403879
___ir_block_131:                            ; offs: 0x403879
                movq xmm0, qword [rsp]
                divsd xmm0, qword [rbp - 8 - 0]
                movq qword [rsp], xmm0
___ir_block_132:                            ; offs: 0x40388b
                movq xmm0, qword [rsp]
                add rsp, 8
                call doubleio_out
___ir_block_133:                            ; offs: 0x40389c
___ir_block_134:                            ; offs: 0x40389c
                mov rdx, 0x0
                movq xmm0, rdx
___ir_block_135:                            ; offs: 0x4038ab
                leave
                ret
___ir_block_136:                            ; offs: 0x4038ad
                leave
                pxor xmm0, xmm0
                ret
; ------------------------- Function definition end -----------------------



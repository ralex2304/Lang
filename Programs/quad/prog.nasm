___ir_block_1:
%include "doubleiolib.nasm"

section .data

GLOBAL_SECTION: times 0 dq 0

section .rodata

align 8
EPSILON: dq 0x3eb0c6f7a0b5ed8d ; 1e-06
align 16
DOUBLE_NEG_CONST: dq -1 >> 1, 0

; Program start

section .text

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
                enter 24, 0

___ir_block_5:
                call doubleio_in
                movq qword [rbp - 8 - 0], xmm0
___ir_block_6:
___ir_block_7:
                call doubleio_in
                movq qword [rbp - 8 - 8], xmm0
___ir_block_8:
___ir_block_9:
                call doubleio_in
                movq qword [rbp - 8 - 16], xmm0
___ir_block_10:
___ir_block_11:
___ir_block_12:
                mov rdx, qword [rbp - 8 - 0]
                mov qword [rsp - 8 - 16 - 0], rdx
___ir_block_13:
___ir_block_14:
                mov rdx, qword [rbp - 8 - 8]
                mov qword [rsp - 8 - 16 - 8], rdx
___ir_block_15:
___ir_block_16:
                mov rdx, qword [rbp - 8 - 16]
                mov qword [rsp - 8 - 16 - 16], rdx
___ir_block_17:
                call ___func_22
                ; func call end

___ir_block_18:
___ir_block_19:
___ir_block_20:
                leave
                ret

___ir_block_21:
                leave
                pxor xmm0, xmm0
                ret
; ------------------------- Function definition end -----------------------


___ir_block_22:
; =========================== Function definition =========================
___func_22:
                enter 32, 0

___ir_block_23:
___ir_block_24:
___ir_block_25:
                ; operands comparison: op1 == op2
                movq xmm1, qword [rbp - 8 - 0]
                mov rdx, 0x0
                movq xmm2, rdx
                add rsp, 18446744073709551608
                movq xmm3, xmm1
                subsd xmm3, xmm2
                andpd xmm3, [DOUBLE_NEG_CONST]
                ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2)

                comisd xmm3, [EPSILON] ; fabs(op1 - op2) {'<' | '>'} EPSILON
                jnc ___compare_25_false

                mov rdx, 0x3ff0000000000000 ; 1
                jmp ___compare_25_end

___compare_25_false:
                mov rdx, 0x0 ; 0
___compare_25_end:
                mov qword [rsp], rdx

___ir_block_26:
                mov rdx, -1 >> 1
                and qword [rsp], rdx
                movq xmm1, qword [rsp]
                add rsp, 8
                comisd xmm1, [EPSILON]
___ir_block_27:
                jc ___ir_block_36
___ir_block_28:
___ir_block_29:
                mov rdx, qword [rbp - 8 - 8]
                mov qword [rsp - 8 - 16 - 0], rdx
___ir_block_30:
___ir_block_31:
                mov rdx, qword [rbp - 8 - 16]
                mov qword [rsp - 8 - 16 - 8], rdx
___ir_block_32:
                call ___func_103
                ; func call end

___ir_block_33:
___ir_block_34:
___ir_block_35:
                leave
                ret

___ir_block_36:
___ir_block_37:
___ir_block_38:
___ir_block_39:
                sub rsp, 8
                movq xmm0, qword [rbp - 8 - 8]
                mulsd xmm0, qword [rbp - 8 - 8]
                movq qword [rsp], xmm0
___ir_block_40:
                sub rsp, 8
                mov rdx, 0x4010000000000000 ; 4
                mov qword [rsp], rdx
___ir_block_41:
___ir_block_42:
___ir_block_43:
                sub rsp, 8
                movq xmm0, qword [rbp - 8 - 0]
                mulsd xmm0, qword [rbp - 8 - 16]
                movq qword [rsp], xmm0
___ir_block_44:
                movq xmm0, [rsp + 8]
                mulsd xmm0, qword [rsp]
                add rsp, 8
                movq qword [rsp], xmm0
___ir_block_45:
                movq xmm0, [rsp + 8]
                subsd xmm0, qword [rsp]
                add rsp, 16
                movq qword [rbp - 8 - 24], xmm0
___ir_block_46:
___ir_block_47:
___ir_block_48:
___ir_block_49:
                ; operands comparison: op1 < op2
                movq xmm1, qword [rbp - 8 - 24]
                mov rdx, 0x0
                movq xmm2, rdx
                add rsp, 18446744073709551608
                movq xmm3, xmm1
                subsd xmm3, xmm2
                andpd xmm3, [DOUBLE_NEG_CONST]
                ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2)

                comisd xmm3, [EPSILON] ; fabs(op1 - op2) {'<' | '>'} EPSILON
                jc ___compare_49_false

                comisd xmm1, xmm2 ; op1 {'<' | '>'} op2
                jnc ___compare_49_false

                mov rdx, 0x3ff0000000000000 ; 1
                jmp ___compare_49_end

___compare_49_false:
                mov rdx, 0x0 ; 0
___compare_49_end:
                mov qword [rsp], rdx

___ir_block_50:
                mov rdx, -1 >> 1
                and qword [rsp], rdx
                movq xmm1, qword [rsp]
                add rsp, 8
                comisd xmm1, [EPSILON]
___ir_block_51:
                jc ___ir_block_57
___ir_block_52:
___ir_block_53:
                mov rdx, 0x0 ; 0
                movq xmm0, rdx
                call doubleio_out
___ir_block_54:
___ir_block_55:
                mov qword rdx, 0x0 ; 0
                movq xmm0, rdx
___ir_block_56:
                leave
                ret

___ir_block_57:
___ir_block_58:
___ir_block_59:
___ir_block_60:
                ; operands comparison: op1 == op2
                movq xmm1, qword [rbp - 8 - 24]
                mov rdx, 0x0
                movq xmm2, rdx
                add rsp, 18446744073709551608
                movq xmm3, xmm1
                subsd xmm3, xmm2
                andpd xmm3, [DOUBLE_NEG_CONST]
                ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2)

                comisd xmm3, [EPSILON] ; fabs(op1 - op2) {'<' | '>'} EPSILON
                jnc ___compare_60_false

                mov rdx, 0x3ff0000000000000 ; 1
                jmp ___compare_60_end

___compare_60_false:
                mov rdx, 0x0 ; 0
___compare_60_end:
                mov qword [rsp], rdx

___ir_block_61:
                mov rdx, -1 >> 1
                and qword [rsp], rdx
                movq xmm1, qword [rsp]
                add rsp, 8
                comisd xmm1, [EPSILON]
___ir_block_62:
                jc ___ir_block_75
___ir_block_63:
___ir_block_64:
                mov rdx, 0x3ff0000000000000 ; 1
                movq xmm0, rdx
                call doubleio_out
___ir_block_65:
___ir_block_66:
                mov rdx, 9223372036854775808
                sub rsp, 8
                mov r8, qword [rbp - 8 - 8]
                mov qword [rsp], r8
                xor qword [rsp], rdx
___ir_block_67:
                sub rsp, 8
                mov rdx, 0x4000000000000000 ; 2
                mov qword [rsp], rdx
___ir_block_68:
___ir_block_69:
                movq xmm0, qword [rsp]
                mulsd xmm0, qword [rbp - 8 - 0]
                movq qword [rsp], xmm0
___ir_block_70:
                movq xmm0, [rsp + 8]
                divsd xmm0, qword [rsp]
                add rsp, 8
                movq qword [rsp], xmm0
___ir_block_71:
                movq xmm0, qword [rsp]
                add rsp, 8
                call doubleio_out
___ir_block_72:
___ir_block_73:
                mov qword rdx, 0x0 ; 0
                movq xmm0, rdx
___ir_block_74:
                leave
                ret

___ir_block_75:
___ir_block_76:
___ir_block_77:
                sqrtsd xmm0, qword [rbp - 8 - 24]
                movq qword [rbp - 8 - 24], xmm0
___ir_block_78:
___ir_block_79:
___ir_block_80:
                mov rdx, 0x4000000000000000 ; 2
                movq xmm0, rdx
                call doubleio_out
___ir_block_81:
___ir_block_82:
                mov rdx, 9223372036854775808
                sub rsp, 8
                mov r8, qword [rbp - 8 - 8]
                mov qword [rsp], r8
                xor qword [rsp], rdx
___ir_block_83:
___ir_block_84:
                movq xmm0, qword [rsp]
                subsd xmm0, qword [rbp - 8 - 24]
                movq qword [rsp], xmm0
___ir_block_85:
                sub rsp, 8
                mov rdx, 0x4000000000000000 ; 2
                mov qword [rsp], rdx
___ir_block_86:
___ir_block_87:
                movq xmm0, qword [rsp]
                mulsd xmm0, qword [rbp - 8 - 0]
                movq qword [rsp], xmm0
___ir_block_88:
                movq xmm0, [rsp + 8]
                divsd xmm0, qword [rsp]
                add rsp, 8
                movq qword [rsp], xmm0
___ir_block_89:
                movq xmm0, qword [rsp]
                add rsp, 8
                call doubleio_out
___ir_block_90:
___ir_block_91:
                mov rdx, 9223372036854775808
                sub rsp, 8
                mov r8, qword [rbp - 8 - 8]
                mov qword [rsp], r8
                xor qword [rsp], rdx
___ir_block_92:
___ir_block_93:
                movq xmm0, qword [rsp]
                addsd xmm0, qword [rbp - 8 - 24]
                movq qword [rsp], xmm0
___ir_block_94:
                sub rsp, 8
                mov rdx, 0x4000000000000000 ; 2
                mov qword [rsp], rdx
___ir_block_95:
___ir_block_96:
                movq xmm0, qword [rsp]
                mulsd xmm0, qword [rbp - 8 - 0]
                movq qword [rsp], xmm0
___ir_block_97:
                movq xmm0, [rsp + 8]
                divsd xmm0, qword [rsp]
                add rsp, 8
                movq qword [rsp], xmm0
___ir_block_98:
                movq xmm0, qword [rsp]
                add rsp, 8
                call doubleio_out
___ir_block_99:
___ir_block_100:
                mov qword rdx, 0x0 ; 0
                movq xmm0, rdx
___ir_block_101:
                leave
                ret

___ir_block_102:
                leave
                pxor xmm0, xmm0
                ret
; ------------------------- Function definition end -----------------------


___ir_block_103:
; =========================== Function definition =========================
___func_103:
                enter 16, 0

___ir_block_104:
___ir_block_105:
___ir_block_106:
                ; operands comparison: op1 == op2
                movq xmm1, qword [rbp - 8 - 0]
                mov rdx, 0x0
                movq xmm2, rdx
                add rsp, 18446744073709551608
                movq xmm3, xmm1
                subsd xmm3, xmm2
                andpd xmm3, [DOUBLE_NEG_CONST]
                ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2)

                comisd xmm3, [EPSILON] ; fabs(op1 - op2) {'<' | '>'} EPSILON
                jnc ___compare_106_false

                mov rdx, 0x3ff0000000000000 ; 1
                jmp ___compare_106_end

___compare_106_false:
                mov rdx, 0x0 ; 0
___compare_106_end:
                mov qword [rsp], rdx

___ir_block_107:
                mov rdx, -1 >> 1
                and qword [rsp], rdx
                movq xmm1, qword [rsp]
                add rsp, 8
                comisd xmm1, [EPSILON]
___ir_block_108:
                jc ___ir_block_125
___ir_block_109:
___ir_block_110:
___ir_block_111:
                ; operands comparison: op1 == op2
                movq xmm1, qword [rbp - 8 - 8]
                mov rdx, 0x0
                movq xmm2, rdx
                add rsp, 18446744073709551608
                movq xmm3, xmm1
                subsd xmm3, xmm2
                andpd xmm3, [DOUBLE_NEG_CONST]
                ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2)

                comisd xmm3, [EPSILON] ; fabs(op1 - op2) {'<' | '>'} EPSILON
                jnc ___compare_111_false

                mov rdx, 0x3ff0000000000000 ; 1
                jmp ___compare_111_end

___compare_111_false:
                mov rdx, 0x0 ; 0
___compare_111_end:
                mov qword [rsp], rdx

___ir_block_112:
                mov rdx, -1 >> 1
                and qword [rsp], rdx
                movq xmm1, qword [rsp]
                add rsp, 8
                comisd xmm1, [EPSILON]
___ir_block_113:
                jc ___ir_block_119
___ir_block_114:
___ir_block_115:
                mov rdx, 0xbff0000000000000 ; -1
                movq xmm0, rdx
                call doubleio_out
___ir_block_116:
___ir_block_117:
                mov qword rdx, 0x0 ; 0
                movq xmm0, rdx
___ir_block_118:
                leave
                ret

___ir_block_119:
___ir_block_120:
___ir_block_121:
                mov rdx, 0x0 ; 0
                movq xmm0, rdx
                call doubleio_out
___ir_block_122:
___ir_block_123:
                mov qword rdx, 0x0 ; 0
                movq xmm0, rdx
___ir_block_124:
                leave
                ret

___ir_block_125:
___ir_block_126:
___ir_block_127:
                mov rdx, 0x3ff0000000000000 ; 1
                movq xmm0, rdx
                call doubleio_out
___ir_block_128:
___ir_block_129:
                mov rdx, 9223372036854775808
                sub rsp, 8
                mov r8, qword [rbp - 8 - 8]
                mov qword [rsp], r8
                xor qword [rsp], rdx
___ir_block_130:
___ir_block_131:
                movq xmm0, qword [rsp]
                divsd xmm0, qword [rbp - 8 - 0]
                movq qword [rsp], xmm0
___ir_block_132:
                movq xmm0, qword [rsp]
                add rsp, 8
                call doubleio_out
___ir_block_133:
___ir_block_134:
                mov qword rdx, 0x0 ; 0
                movq xmm0, rdx
___ir_block_135:
                leave
                ret

___ir_block_136:
                leave
                pxor xmm0, xmm0
                ret
; ------------------------- Function definition end -----------------------



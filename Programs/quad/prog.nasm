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
                enter 24, 0

___ir_block_5:
                call doubleio_in
                sub rsp, 8
                movsd qword [rsp], xmm0
___ir_block_6:
                mov rdx, [rsp]
                add rsp, 8
                mov qword [rbp - 8 - 0], rdx
___ir_block_7:
                call doubleio_in
                sub rsp, 8
                movsd qword [rsp], xmm0
___ir_block_8:
                mov rdx, [rsp]
                add rsp, 8
                mov qword [rbp - 8 - 8], rdx
___ir_block_9:
                call doubleio_in
                sub rsp, 8
                movsd qword [rsp], xmm0
___ir_block_10:
                mov rdx, [rsp]
                add rsp, 8
                mov qword [rbp - 8 - 16], rdx
___ir_block_11:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 0]
                mov qword [rsp], rdx
___ir_block_12:
                mov rdx, [rsp]
                add rsp, 8
                mov qword [rsp - 8 - 16 - 0], rdx
___ir_block_13:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 8]
                mov qword [rsp], rdx
___ir_block_14:
                mov rdx, [rsp]
                add rsp, 8
                mov qword [rsp - 8 - 16 - 8], rdx
___ir_block_15:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 16]
                mov qword [rsp], rdx
___ir_block_16:
                mov rdx, [rsp]
                add rsp, 8
                mov qword [rsp - 8 - 16 - 16], rdx
___ir_block_17:
                call ___func_22
                ; func call end

___ir_block_18:
                sub rsp, 8
                movsd qword [rsp], xmm0
___ir_block_19:
                movsd xmm0, [rsp]
                add rsp, 8
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
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 0]
                mov qword [rsp], rdx
___ir_block_24:
                sub rsp, 8
                mov rdx, 0x0 ; 0
                mov qword [rsp], rdx
___ir_block_25:
                ; operands comparison: op1 == op2
                movsd xmm2, [rsp]
                movsd xmm1, [rsp + 8]
                movsd xmm3, xmm1
                subsd xmm3, xmm2
                movsd [rsp], xmm3
                mov rdx, -1 >> 1
                and qword [rsp], rdx
                movsd xmm3, [rsp]
                add rsp, 16
                ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2)

                comisd xmm3, [EPSILON] ; fabs(op1 - op2) {'<' | '>'} EPSILON
                jnc ___compare_25_false

                sub rsp, 8
                mov rdx, 0x3ff0000000000000 ; 1
                mov qword [rsp], rdx
                jmp ___compare_25_end

___compare_25_false:
                sub rsp, 8
                mov rdx, 0x0 ; 0
                mov qword [rsp], rdx
___compare_25_end:

___ir_block_26:
                mov rdx, -1 >> 1
                and qword [rsp], rdx
                movsd xmm1, qword [rsp]
                add rsp, 8
                comisd xmm1, [EPSILON]
___ir_block_27:
                jc ___ir_block_36
___ir_block_28:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 8]
                mov qword [rsp], rdx
___ir_block_29:
                mov rdx, [rsp]
                add rsp, 8
                mov qword [rsp - 8 - 16 - 0], rdx
___ir_block_30:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 16]
                mov qword [rsp], rdx
___ir_block_31:
                mov rdx, [rsp]
                add rsp, 8
                mov qword [rsp - 8 - 16 - 8], rdx
___ir_block_32:
                call ___func_103
                ; func call end

___ir_block_33:
                sub rsp, 8
                movsd qword [rsp], xmm0
___ir_block_34:
                movsd xmm0, [rsp]
                add rsp, 8
___ir_block_35:
                leave
                ret

___ir_block_36:
___ir_block_37:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 8]
                mov qword [rsp], rdx
___ir_block_38:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 8]
                mov qword [rsp], rdx
___ir_block_39:
                movsd xmm0, [rsp + 8]
                mulsd xmm0, qword [rsp]
                add rsp, 8
                movsd qword [rsp], xmm0
___ir_block_40:
                sub rsp, 8
                mov rdx, 0x4010000000000000 ; 4
                mov qword [rsp], rdx
___ir_block_41:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 0]
                mov qword [rsp], rdx
___ir_block_42:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 16]
                mov qword [rsp], rdx
___ir_block_43:
                movsd xmm0, [rsp + 8]
                mulsd xmm0, qword [rsp]
                add rsp, 8
                movsd qword [rsp], xmm0
___ir_block_44:
                movsd xmm0, [rsp + 8]
                mulsd xmm0, qword [rsp]
                add rsp, 8
                movsd qword [rsp], xmm0
___ir_block_45:
                movsd xmm0, [rsp + 8]
                subsd xmm0, qword [rsp]
                add rsp, 8
                movsd qword [rsp], xmm0
___ir_block_46:
                mov rdx, [rsp]
                add rsp, 8
                mov qword [rbp - 8 - 24], rdx
___ir_block_47:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 24]
                mov qword [rsp], rdx
___ir_block_48:
                sub rsp, 8
                mov rdx, 0x0 ; 0
                mov qword [rsp], rdx
___ir_block_49:
                ; operands comparison: op1 < op2
                movsd xmm2, [rsp]
                movsd xmm1, [rsp + 8]
                movsd xmm3, xmm1
                subsd xmm3, xmm2
                movsd [rsp], xmm3
                mov rdx, -1 >> 1
                and qword [rsp], rdx
                movsd xmm3, [rsp]
                add rsp, 16
                ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2)

                comisd xmm3, [EPSILON] ; fabs(op1 - op2) {'<' | '>'} EPSILON
                jc ___compare_49_false

                comisd xmm1, xmm2 ; op1 {'<' | '>'} op2
                jnc ___compare_49_false

                sub rsp, 8
                mov rdx, 0x3ff0000000000000 ; 1
                mov qword [rsp], rdx
                jmp ___compare_49_end

___compare_49_false:
                sub rsp, 8
                mov rdx, 0x0 ; 0
                mov qword [rsp], rdx
___compare_49_end:

___ir_block_50:
                mov rdx, -1 >> 1
                and qword [rsp], rdx
                movsd xmm1, qword [rsp]
                add rsp, 8
                comisd xmm1, [EPSILON]
___ir_block_51:
                jc ___ir_block_57
___ir_block_52:
                sub rsp, 8
                mov rdx, 0x0 ; 0
                mov qword [rsp], rdx
___ir_block_53:
                movsd xmm0, qword [rsp]
                add rsp, 8
                call doubleio_out
___ir_block_54:
                sub rsp, 8
                mov rdx, 0x0 ; 0
                mov qword [rsp], rdx
___ir_block_55:
                movsd xmm0, [rsp]
                add rsp, 8
___ir_block_56:
                leave
                ret

___ir_block_57:
___ir_block_58:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 24]
                mov qword [rsp], rdx
___ir_block_59:
                sub rsp, 8
                mov rdx, 0x0 ; 0
                mov qword [rsp], rdx
___ir_block_60:
                ; operands comparison: op1 == op2
                movsd xmm2, [rsp]
                movsd xmm1, [rsp + 8]
                movsd xmm3, xmm1
                subsd xmm3, xmm2
                movsd [rsp], xmm3
                mov rdx, -1 >> 1
                and qword [rsp], rdx
                movsd xmm3, [rsp]
                add rsp, 16
                ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2)

                comisd xmm3, [EPSILON] ; fabs(op1 - op2) {'<' | '>'} EPSILON
                jnc ___compare_60_false

                sub rsp, 8
                mov rdx, 0x3ff0000000000000 ; 1
                mov qword [rsp], rdx
                jmp ___compare_60_end

___compare_60_false:
                sub rsp, 8
                mov rdx, 0x0 ; 0
                mov qword [rsp], rdx
___compare_60_end:

___ir_block_61:
                mov rdx, -1 >> 1
                and qword [rsp], rdx
                movsd xmm1, qword [rsp]
                add rsp, 8
                comisd xmm1, [EPSILON]
___ir_block_62:
                jc ___ir_block_75
___ir_block_63:
                sub rsp, 8
                mov rdx, 0x3ff0000000000000 ; 1
                mov qword [rsp], rdx
___ir_block_64:
                movsd xmm0, qword [rsp]
                add rsp, 8
                call doubleio_out
___ir_block_65:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 8]
                mov qword [rsp], rdx
___ir_block_66:
                mov rdx, 9223372036854775808
                xor qword [rsp], rdx
___ir_block_67:
                sub rsp, 8
                mov rdx, 0x4000000000000000 ; 2
                mov qword [rsp], rdx
___ir_block_68:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 0]
                mov qword [rsp], rdx
___ir_block_69:
                movsd xmm0, [rsp + 8]
                mulsd xmm0, qword [rsp]
                add rsp, 8
                movsd qword [rsp], xmm0
___ir_block_70:
                movsd xmm0, [rsp + 8]
                divsd xmm0, qword [rsp]
                add rsp, 8
                movsd qword [rsp], xmm0
___ir_block_71:
                movsd xmm0, qword [rsp]
                add rsp, 8
                call doubleio_out
___ir_block_72:
                sub rsp, 8
                mov rdx, 0x0 ; 0
                mov qword [rsp], rdx
___ir_block_73:
                movsd xmm0, [rsp]
                add rsp, 8
___ir_block_74:
                leave
                ret

___ir_block_75:
___ir_block_76:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 24]
                mov qword [rsp], rdx
___ir_block_77:
                sqrtsd xmm0, qword [rsp]
                movsd qword [rsp], xmm0
___ir_block_78:
                mov rdx, [rsp]
                add rsp, 8
                mov qword [rbp - 8 - 24], rdx
___ir_block_79:
                sub rsp, 8
                mov rdx, 0x4000000000000000 ; 2
                mov qword [rsp], rdx
___ir_block_80:
                movsd xmm0, qword [rsp]
                add rsp, 8
                call doubleio_out
___ir_block_81:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 8]
                mov qword [rsp], rdx
___ir_block_82:
                mov rdx, 9223372036854775808
                xor qword [rsp], rdx
___ir_block_83:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 24]
                mov qword [rsp], rdx
___ir_block_84:
                movsd xmm0, [rsp + 8]
                subsd xmm0, qword [rsp]
                add rsp, 8
                movsd qword [rsp], xmm0
___ir_block_85:
                sub rsp, 8
                mov rdx, 0x4000000000000000 ; 2
                mov qword [rsp], rdx
___ir_block_86:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 0]
                mov qword [rsp], rdx
___ir_block_87:
                movsd xmm0, [rsp + 8]
                mulsd xmm0, qword [rsp]
                add rsp, 8
                movsd qword [rsp], xmm0
___ir_block_88:
                movsd xmm0, [rsp + 8]
                divsd xmm0, qword [rsp]
                add rsp, 8
                movsd qword [rsp], xmm0
___ir_block_89:
                movsd xmm0, qword [rsp]
                add rsp, 8
                call doubleio_out
___ir_block_90:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 8]
                mov qword [rsp], rdx
___ir_block_91:
                mov rdx, 9223372036854775808
                xor qword [rsp], rdx
___ir_block_92:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 24]
                mov qword [rsp], rdx
___ir_block_93:
                movsd xmm0, [rsp + 8]
                addsd xmm0, qword [rsp]
                add rsp, 8
                movsd qword [rsp], xmm0
___ir_block_94:
                sub rsp, 8
                mov rdx, 0x4000000000000000 ; 2
                mov qword [rsp], rdx
___ir_block_95:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 0]
                mov qword [rsp], rdx
___ir_block_96:
                movsd xmm0, [rsp + 8]
                mulsd xmm0, qword [rsp]
                add rsp, 8
                movsd qword [rsp], xmm0
___ir_block_97:
                movsd xmm0, [rsp + 8]
                divsd xmm0, qword [rsp]
                add rsp, 8
                movsd qword [rsp], xmm0
___ir_block_98:
                movsd xmm0, qword [rsp]
                add rsp, 8
                call doubleio_out
___ir_block_99:
                sub rsp, 8
                mov rdx, 0x0 ; 0
                mov qword [rsp], rdx
___ir_block_100:
                movsd xmm0, [rsp]
                add rsp, 8
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
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 0]
                mov qword [rsp], rdx
___ir_block_105:
                sub rsp, 8
                mov rdx, 0x0 ; 0
                mov qword [rsp], rdx
___ir_block_106:
                ; operands comparison: op1 == op2
                movsd xmm2, [rsp]
                movsd xmm1, [rsp + 8]
                movsd xmm3, xmm1
                subsd xmm3, xmm2
                movsd [rsp], xmm3
                mov rdx, -1 >> 1
                and qword [rsp], rdx
                movsd xmm3, [rsp]
                add rsp, 16
                ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2)

                comisd xmm3, [EPSILON] ; fabs(op1 - op2) {'<' | '>'} EPSILON
                jnc ___compare_106_false

                sub rsp, 8
                mov rdx, 0x3ff0000000000000 ; 1
                mov qword [rsp], rdx
                jmp ___compare_106_end

___compare_106_false:
                sub rsp, 8
                mov rdx, 0x0 ; 0
                mov qword [rsp], rdx
___compare_106_end:

___ir_block_107:
                mov rdx, -1 >> 1
                and qword [rsp], rdx
                movsd xmm1, qword [rsp]
                add rsp, 8
                comisd xmm1, [EPSILON]
___ir_block_108:
                jc ___ir_block_125
___ir_block_109:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 8]
                mov qword [rsp], rdx
___ir_block_110:
                sub rsp, 8
                mov rdx, 0x0 ; 0
                mov qword [rsp], rdx
___ir_block_111:
                ; operands comparison: op1 == op2
                movsd xmm2, [rsp]
                movsd xmm1, [rsp + 8]
                movsd xmm3, xmm1
                subsd xmm3, xmm2
                movsd [rsp], xmm3
                mov rdx, -1 >> 1
                and qword [rsp], rdx
                movsd xmm3, [rsp]
                add rsp, 16
                ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2)

                comisd xmm3, [EPSILON] ; fabs(op1 - op2) {'<' | '>'} EPSILON
                jnc ___compare_111_false

                sub rsp, 8
                mov rdx, 0x3ff0000000000000 ; 1
                mov qword [rsp], rdx
                jmp ___compare_111_end

___compare_111_false:
                sub rsp, 8
                mov rdx, 0x0 ; 0
                mov qword [rsp], rdx
___compare_111_end:

___ir_block_112:
                mov rdx, -1 >> 1
                and qword [rsp], rdx
                movsd xmm1, qword [rsp]
                add rsp, 8
                comisd xmm1, [EPSILON]
___ir_block_113:
                jc ___ir_block_119
___ir_block_114:
                sub rsp, 8
                mov rdx, 0xbff0000000000000 ; -1
                mov qword [rsp], rdx
___ir_block_115:
                movsd xmm0, qword [rsp]
                add rsp, 8
                call doubleio_out
___ir_block_116:
                sub rsp, 8
                mov rdx, 0x0 ; 0
                mov qword [rsp], rdx
___ir_block_117:
                movsd xmm0, [rsp]
                add rsp, 8
___ir_block_118:
                leave
                ret

___ir_block_119:
___ir_block_120:
                sub rsp, 8
                mov rdx, 0x0 ; 0
                mov qword [rsp], rdx
___ir_block_121:
                movsd xmm0, qword [rsp]
                add rsp, 8
                call doubleio_out
___ir_block_122:
                sub rsp, 8
                mov rdx, 0x0 ; 0
                mov qword [rsp], rdx
___ir_block_123:
                movsd xmm0, [rsp]
                add rsp, 8
___ir_block_124:
                leave
                ret

___ir_block_125:
___ir_block_126:
                sub rsp, 8
                mov rdx, 0x3ff0000000000000 ; 1
                mov qword [rsp], rdx
___ir_block_127:
                movsd xmm0, qword [rsp]
                add rsp, 8
                call doubleio_out
___ir_block_128:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 8]
                mov qword [rsp], rdx
___ir_block_129:
                mov rdx, 9223372036854775808
                xor qword [rsp], rdx
___ir_block_130:
                sub rsp, 8
                mov rdx, qword [rbp - 8 - 0]
                mov qword [rsp], rdx
___ir_block_131:
                movsd xmm0, [rsp + 8]
                divsd xmm0, qword [rsp]
                add rsp, 8
                movsd qword [rsp], xmm0
___ir_block_132:
                movsd xmm0, qword [rsp]
                add rsp, 8
                call doubleio_out
___ir_block_133:
                sub rsp, 8
                mov rdx, 0x0 ; 0
                mov qword [rsp], rdx
___ir_block_134:
                movsd xmm0, [rsp]
                add rsp, 8
___ir_block_135:
                leave
                ret

___ir_block_136:
                leave
                pxor xmm0, xmm0
                ret
; ------------------------- Function definition end -----------------------


___ir_block_137:
%include "doubleiolib.nasm"

section .data

GLOBAL_SECTION: times 0 dq 0

section .rodata

EPSILON: dq 0x3eb0c6f7a0b5ed8d ; 1e-06


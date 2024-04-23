; Program start

section .text

extern printf
extern scanf
global main

main:
        enter 0, 0
        ; func call: остров_в_океане
        call ___func_0
        ; func call end

        leave
        mov rax, 0
        ret

; =========================== Function definition =========================
; func name: остров_в_океане
___func_0:
        enter 0x0018, 0; this place is patched in the end of function definition

        sub rsp, 8
        lea rdi, [SCANF_DOUBLE_FMT]
        lea rsi, [rsp]
        call scanf

        ; var assignment: А
        pop qword [rbp - 8 - 0]

        sub rsp, 8
        lea rdi, [SCANF_DOUBLE_FMT]
        lea rsi, [rsp]
        call scanf

        ; var assignment: Б
        pop qword [rbp - 8 - 8]

        sub rsp, 8
        lea rdi, [SCANF_DOUBLE_FMT]
        lea rsi, [rsp]
        call scanf

        ; var assignment: В
        pop qword [rbp - 8 - 16]

        push qword [rbp - 8 - 0]
        pop qword [rbp - 8 - 16 - 24]

        push qword [rbp - 8 - 8]
        pop qword [rbp - 8 - 16 - 32]

        push qword [rbp - 8 - 16]
        pop qword [rbp - 8 - 16 - 40]

        ; func call: квадратка
        call ___func_4
        ; func call end

        sub rsp, 8
        movsd [rsp], xmm0

        movsd xmm0, [rsp]
        add rsp, 8
        leave
        ret

        leave
        ret
; ------------------------- Function definition end -----------------------


; =========================== Function definition =========================
; func name: квадратка
___func_4:
        enter 0x0020, 0; this place is patched in the end of function definition

        push qword [rbp - 8 - 0]
        sub rsp, 8
        mov rdx, 0x0
        mov qword [rsp], rdx
        ; operands comparison: op1 == op2
        movsd xmm2, [rsp]
        movsd xmm1, [rsp + 8]
        add rsp, 8
        movsd xmm3, xmm1
        subsd xmm3, xmm2
        movsd [rsp], xmm3
        mov rdx, -1 >> 1 ; 0x7FFFFFFFFFFFFFFF
        and qword [rsp], rdx
        movsd xmm3, [rsp]
        sub rsp, 8
        mov rdx, 0x3eb0c6f7a0b5ed8d
        mov qword [rsp], rdx
        movsd xmm4, [rsp]
        add rsp, 8
        ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2); xmm4 - EPSILON

        comisd xmm3, xmm4 ; fabs(op1 - op2) {'<' | '>'} EPSILON
        jnc ___compare_1_false

        sub rsp, 8
        mov rdx, 0x3ff0000000000000
        mov qword [rsp], rdx
        jmp ___compare_1_end

___compare_1_false:
        sub rsp, 8
        mov rdx, 0x0
        mov qword [rsp], rdx
___compare_1_end:

        ; if begin
        sub rsp, 8
        mov rdx, 0x3eb0c6f7a0b5ed8d
        mov qword [rsp], rdx
        mov rdx, -1 >> 1 ; 0x7FFFFFFFFFFFFFFF
        and qword [rsp + 8], rdx
        movsd xmm1, [rsp + 8]
        movsd xmm2, [rsp]
        add rsp, 16
        comisd xmm1, xmm2
        jc ___if_5_end
        push qword [rbp - 8 - 8]
        pop qword [rbp - 8 - 16 - 24]

        push qword [rbp - 8 - 16]
        pop qword [rbp - 8 - 16 - 32]

        ; func call: не_квадратка
        call ___func_5
        ; func call end

        sub rsp, 8
        movsd [rsp], xmm0

        movsd xmm0, [rsp]
        add rsp, 8
        leave
        ret

___if_5_end:
        ; if end

        push qword [rbp - 8 - 8]
        push qword [rbp - 8 - 8]
        movsd xmm2, [rsp]
        movsd xmm1, [rsp + 8]
        add rsp, 8
        mulsd xmm1, xmm2
        movsd [rsp], xmm1

        sub rsp, 8
        mov rdx, 0x4010000000000000
        mov qword [rsp], rdx
        push qword [rbp - 8 - 0]
        push qword [rbp - 8 - 16]
        movsd xmm2, [rsp]
        movsd xmm1, [rsp + 8]
        add rsp, 8
        mulsd xmm1, xmm2
        movsd [rsp], xmm1

        movsd xmm2, [rsp]
        movsd xmm1, [rsp + 8]
        add rsp, 8
        mulsd xmm1, xmm2
        movsd [rsp], xmm1

        movsd xmm2, [rsp]
        movsd xmm1, [rsp + 8]
        add rsp, 8
        subsd xmm1, xmm2
        movsd [rsp], xmm1

        ; var assignment: дискр
        pop qword [rbp - 8 - 24]

        push qword [rbp - 8 - 24]
        sub rsp, 8
        mov rdx, 0x0
        mov qword [rsp], rdx
        ; operands comparison: op1 < op2
        movsd xmm2, [rsp]
        movsd xmm1, [rsp + 8]
        add rsp, 8
        movsd xmm3, xmm1
        subsd xmm3, xmm2
        movsd [rsp], xmm3
        mov rdx, -1 >> 1 ; 0x7FFFFFFFFFFFFFFF
        and qword [rsp], rdx
        movsd xmm3, [rsp]
        sub rsp, 8
        mov rdx, 0x3eb0c6f7a0b5ed8d
        mov qword [rsp], rdx
        movsd xmm4, [rsp]
        add rsp, 8
        ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2); xmm4 - EPSILON

        comisd xmm3, xmm4 ; fabs(op1 - op2) {'<' | '>'} EPSILON
        jc ___compare_2_false

        comisd xmm1, xmm2 ; op1 {'<' | '>'} op2
        jnc ___compare_2_false

        sub rsp, 8
        mov rdx, 0x3ff0000000000000
        mov qword [rsp], rdx
        jmp ___compare_2_end

___compare_2_false:
        sub rsp, 8
        mov rdx, 0x0
        mov qword [rsp], rdx
___compare_2_end:

        ; if begin
        sub rsp, 8
        mov rdx, 0x3eb0c6f7a0b5ed8d
        mov qword [rsp], rdx
        mov rdx, -1 >> 1 ; 0x7FFFFFFFFFFFFFFF
        and qword [rsp + 8], rdx
        movsd xmm1, [rsp + 8]
        movsd xmm2, [rsp]
        add rsp, 16
        comisd xmm1, xmm2
        jc ___if_6_end
        sub rsp, 8
        mov rdx, 0x0
        mov qword [rsp], rdx
        ; printf
        lea rdi, [PRINTF_DOUBLE_FMT]
        movsd xmm0, [rsp]
        add rsp, 8

        mov rdx, 0x0F
        and rdx, rsp
        test rdx, rdx
        je is_aligned_0

        sub rsp, 8
        call printf
        add rsp, 8
        jmp is_aligned_end_0

is_aligned_0:
        call printf
is_aligned_end_0:

        sub rsp, 8
        mov rdx, 0x0
        mov qword [rsp], rdx
        movsd xmm0, [rsp]
        add rsp, 8
        leave
        ret

___if_6_end:
        ; if end

        push qword [rbp - 8 - 24]
        sub rsp, 8
        mov rdx, 0x0
        mov qword [rsp], rdx
        ; operands comparison: op1 == op2
        movsd xmm2, [rsp]
        movsd xmm1, [rsp + 8]
        add rsp, 8
        movsd xmm3, xmm1
        subsd xmm3, xmm2
        movsd [rsp], xmm3
        mov rdx, -1 >> 1 ; 0x7FFFFFFFFFFFFFFF
        and qword [rsp], rdx
        movsd xmm3, [rsp]
        sub rsp, 8
        mov rdx, 0x3eb0c6f7a0b5ed8d
        mov qword [rsp], rdx
        movsd xmm4, [rsp]
        add rsp, 8
        ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2); xmm4 - EPSILON

        comisd xmm3, xmm4 ; fabs(op1 - op2) {'<' | '>'} EPSILON
        jnc ___compare_3_false

        sub rsp, 8
        mov rdx, 0x3ff0000000000000
        mov qword [rsp], rdx
        jmp ___compare_3_end

___compare_3_false:
        sub rsp, 8
        mov rdx, 0x0
        mov qword [rsp], rdx
___compare_3_end:

        ; if begin
        sub rsp, 8
        mov rdx, 0x3eb0c6f7a0b5ed8d
        mov qword [rsp], rdx
        mov rdx, -1 >> 1 ; 0x7FFFFFFFFFFFFFFF
        and qword [rsp + 8], rdx
        movsd xmm1, [rsp + 8]
        movsd xmm2, [rsp]
        add rsp, 16
        comisd xmm1, xmm2
        jc ___if_7_end
        sub rsp, 8
        mov rdx, 0x3ff0000000000000
        mov qword [rsp], rdx
        ; printf
        lea rdi, [PRINTF_DOUBLE_FMT]
        movsd xmm0, [rsp]
        add rsp, 8

        mov rdx, 0x0F
        and rdx, rsp
        test rdx, rdx
        je is_aligned_1

        sub rsp, 8
        call printf
        add rsp, 8
        jmp is_aligned_end_1

is_aligned_1:
        call printf
is_aligned_end_1:

        push qword [rbp - 8 - 8]
        mov rdx, 1 << 63
        xor qword [rsp], rdx
        sub rsp, 8
        mov rdx, 0x4000000000000000
        mov qword [rsp], rdx
        push qword [rbp - 8 - 0]
        movsd xmm2, [rsp]
        movsd xmm1, [rsp + 8]
        add rsp, 8
        mulsd xmm1, xmm2
        movsd [rsp], xmm1

        movsd xmm2, [rsp]
        movsd xmm1, [rsp + 8]
        add rsp, 8
        divsd xmm1, xmm2
        movsd [rsp], xmm1

        ; printf
        lea rdi, [PRINTF_DOUBLE_FMT]
        movsd xmm0, [rsp]
        add rsp, 8

        mov rdx, 0x0F
        and rdx, rsp
        test rdx, rdx
        je is_aligned_2

        sub rsp, 8
        call printf
        add rsp, 8
        jmp is_aligned_end_2

is_aligned_2:
        call printf
is_aligned_end_2:

        sub rsp, 8
        mov rdx, 0x0
        mov qword [rsp], rdx
        movsd xmm0, [rsp]
        add rsp, 8
        leave
        ret

___if_7_end:
        ; if end

        push qword [rbp - 8 - 24]
        sqrtsd xmm1 , [rsp]
        movsd [rsp], xmm1
        ; var assignment: дискр
        pop qword [rbp - 8 - 24]

        sub rsp, 8
        mov rdx, 0x4000000000000000
        mov qword [rsp], rdx
        ; printf
        lea rdi, [PRINTF_DOUBLE_FMT]
        movsd xmm0, [rsp]
        add rsp, 8

        mov rdx, 0x0F
        and rdx, rsp
        test rdx, rdx
        je is_aligned_3

        sub rsp, 8
        call printf
        add rsp, 8
        jmp is_aligned_end_3

is_aligned_3:
        call printf
is_aligned_end_3:

        push qword [rbp - 8 - 8]
        mov rdx, 1 << 63
        xor qword [rsp], rdx
        push qword [rbp - 8 - 24]
        movsd xmm2, [rsp]
        movsd xmm1, [rsp + 8]
        add rsp, 8
        subsd xmm1, xmm2
        movsd [rsp], xmm1

        sub rsp, 8
        mov rdx, 0x4000000000000000
        mov qword [rsp], rdx
        push qword [rbp - 8 - 0]
        movsd xmm2, [rsp]
        movsd xmm1, [rsp + 8]
        add rsp, 8
        mulsd xmm1, xmm2
        movsd [rsp], xmm1

        movsd xmm2, [rsp]
        movsd xmm1, [rsp + 8]
        add rsp, 8
        divsd xmm1, xmm2
        movsd [rsp], xmm1

        ; printf
        lea rdi, [PRINTF_DOUBLE_FMT]
        movsd xmm0, [rsp]
        add rsp, 8

        mov rdx, 0x0F
        and rdx, rsp
        test rdx, rdx
        je is_aligned_4

        sub rsp, 8
        call printf
        add rsp, 8
        jmp is_aligned_end_4

is_aligned_4:
        call printf
is_aligned_end_4:

        push qword [rbp - 8 - 8]
        mov rdx, 1 << 63
        xor qword [rsp], rdx
        push qword [rbp - 8 - 24]
        movsd xmm2, [rsp]
        movsd xmm1, [rsp + 8]
        add rsp, 8
        addsd xmm1, xmm2
        movsd [rsp], xmm1

        sub rsp, 8
        mov rdx, 0x4000000000000000
        mov qword [rsp], rdx
        push qword [rbp - 8 - 0]
        movsd xmm2, [rsp]
        movsd xmm1, [rsp + 8]
        add rsp, 8
        mulsd xmm1, xmm2
        movsd [rsp], xmm1

        movsd xmm2, [rsp]
        movsd xmm1, [rsp + 8]
        add rsp, 8
        divsd xmm1, xmm2
        movsd [rsp], xmm1

        ; printf
        lea rdi, [PRINTF_DOUBLE_FMT]
        movsd xmm0, [rsp]
        add rsp, 8

        mov rdx, 0x0F
        and rdx, rsp
        test rdx, rdx
        je is_aligned_5

        sub rsp, 8
        call printf
        add rsp, 8
        jmp is_aligned_end_5

is_aligned_5:
        call printf
is_aligned_end_5:

        sub rsp, 8
        mov rdx, 0x0
        mov qword [rsp], rdx
        movsd xmm0, [rsp]
        add rsp, 8
        leave
        ret

        leave
        ret
; ------------------------- Function definition end -----------------------


; =========================== Function definition =========================
; func name: не_квадратка
___func_5:
        enter 0x0010, 0; this place is patched in the end of function definition

        push qword [rbp - 8 - 0]
        sub rsp, 8
        mov rdx, 0x0
        mov qword [rsp], rdx
        ; operands comparison: op1 == op2
        movsd xmm2, [rsp]
        movsd xmm1, [rsp + 8]
        add rsp, 8
        movsd xmm3, xmm1
        subsd xmm3, xmm2
        movsd [rsp], xmm3
        mov rdx, -1 >> 1 ; 0x7FFFFFFFFFFFFFFF
        and qword [rsp], rdx
        movsd xmm3, [rsp]
        sub rsp, 8
        mov rdx, 0x3eb0c6f7a0b5ed8d
        mov qword [rsp], rdx
        movsd xmm4, [rsp]
        add rsp, 8
        ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2); xmm4 - EPSILON

        comisd xmm3, xmm4 ; fabs(op1 - op2) {'<' | '>'} EPSILON
        jnc ___compare_4_false

        sub rsp, 8
        mov rdx, 0x3ff0000000000000
        mov qword [rsp], rdx
        jmp ___compare_4_end

___compare_4_false:
        sub rsp, 8
        mov rdx, 0x0
        mov qword [rsp], rdx
___compare_4_end:

        ; if begin
        sub rsp, 8
        mov rdx, 0x3eb0c6f7a0b5ed8d
        mov qword [rsp], rdx
        mov rdx, -1 >> 1 ; 0x7FFFFFFFFFFFFFFF
        and qword [rsp + 8], rdx
        movsd xmm1, [rsp + 8]
        movsd xmm2, [rsp]
        add rsp, 16
        comisd xmm1, xmm2
        jc ___if_10_end
        push qword [rbp - 8 - 8]
        sub rsp, 8
        mov rdx, 0x0
        mov qword [rsp], rdx
        ; operands comparison: op1 == op2
        movsd xmm2, [rsp]
        movsd xmm1, [rsp + 8]
        add rsp, 8
        movsd xmm3, xmm1
        subsd xmm3, xmm2
        movsd [rsp], xmm3
        mov rdx, -1 >> 1 ; 0x7FFFFFFFFFFFFFFF
        and qword [rsp], rdx
        movsd xmm3, [rsp]
        sub rsp, 8
        mov rdx, 0x3eb0c6f7a0b5ed8d
        mov qword [rsp], rdx
        movsd xmm4, [rsp]
        add rsp, 8
        ; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2); xmm4 - EPSILON

        comisd xmm3, xmm4 ; fabs(op1 - op2) {'<' | '>'} EPSILON
        jnc ___compare_5_false

        sub rsp, 8
        mov rdx, 0x3ff0000000000000
        mov qword [rsp], rdx
        jmp ___compare_5_end

___compare_5_false:
        sub rsp, 8
        mov rdx, 0x0
        mov qword [rsp], rdx
___compare_5_end:

        ; if begin
        sub rsp, 8
        mov rdx, 0x3eb0c6f7a0b5ed8d
        mov qword [rsp], rdx
        mov rdx, -1 >> 1 ; 0x7FFFFFFFFFFFFFFF
        and qword [rsp + 8], rdx
        movsd xmm1, [rsp + 8]
        movsd xmm2, [rsp]
        add rsp, 16
        comisd xmm1, xmm2
        jc ___if_11_end
        sub rsp, 8
        mov rdx, 0xbff0000000000000
        mov qword [rsp], rdx
        ; printf
        lea rdi, [PRINTF_DOUBLE_FMT]
        movsd xmm0, [rsp]
        add rsp, 8

        mov rdx, 0x0F
        and rdx, rsp
        test rdx, rdx
        je is_aligned_6

        sub rsp, 8
        call printf
        add rsp, 8
        jmp is_aligned_end_6

is_aligned_6:
        call printf
is_aligned_end_6:

        sub rsp, 8
        mov rdx, 0x0
        mov qword [rsp], rdx
        movsd xmm0, [rsp]
        add rsp, 8
        leave
        ret

___if_11_end:
        ; if end

        sub rsp, 8
        mov rdx, 0x0
        mov qword [rsp], rdx
        ; printf
        lea rdi, [PRINTF_DOUBLE_FMT]
        movsd xmm0, [rsp]
        add rsp, 8

        mov rdx, 0x0F
        and rdx, rsp
        test rdx, rdx
        je is_aligned_7

        sub rsp, 8
        call printf
        add rsp, 8
        jmp is_aligned_end_7

is_aligned_7:
        call printf
is_aligned_end_7:

        sub rsp, 8
        mov rdx, 0x0
        mov qword [rsp], rdx
        movsd xmm0, [rsp]
        add rsp, 8
        leave
        ret

___if_10_end:
        ; if end

        sub rsp, 8
        mov rdx, 0x3ff0000000000000
        mov qword [rsp], rdx
        ; printf
        lea rdi, [PRINTF_DOUBLE_FMT]
        movsd xmm0, [rsp]
        add rsp, 8

        mov rdx, 0x0F
        and rdx, rsp
        test rdx, rdx
        je is_aligned_8

        sub rsp, 8
        call printf
        add rsp, 8
        jmp is_aligned_end_8

is_aligned_8:
        call printf
is_aligned_end_8:

        push qword [rbp - 8 - 8]
        mov rdx, 1 << 63
        xor qword [rsp], rdx
        push qword [rbp - 8 - 0]
        movsd xmm2, [rsp]
        movsd xmm1, [rsp + 8]
        add rsp, 8
        divsd xmm1, xmm2
        movsd [rsp], xmm1

        ; printf
        lea rdi, [PRINTF_DOUBLE_FMT]
        movsd xmm0, [rsp]
        add rsp, 8

        mov rdx, 0x0F
        and rdx, rsp
        test rdx, rdx
        je is_aligned_9

        sub rsp, 8
        call printf
        add rsp, 8
        jmp is_aligned_end_9

is_aligned_9:
        call printf
is_aligned_end_9:

        sub rsp, 8
        mov rdx, 0x0
        mov qword [rsp], rdx
        movsd xmm0, [rsp]
        add rsp, 8
        leave
        ret

        leave
        ret
; ------------------------- Function definition end -----------------------


section .data

GLOBAL_SECTION: times 0 dq 0

section .rodata

PRINTF_DOUBLE_FMT: db '%', 'l', 'g', 0x0a, 0
SCANF_DOUBLE_FMT:  db '%', 'l', 'f', 0

section .text

TEXT_BEGIN:

;-------------------------------------------------
; read double
;
; Destr:    rax, rbx, rdx, rdi, rsi, r8, r9, r10, r11, xmm1
;
; Ret:      xmm0
;-------------------------------------------------
doubleio_in:

                mov rax, 0x01               ; sys_write
                mov rdi, 0x01               ; stdout
                lea rsi, [EnterDoubleStr]   ; string
                mov rdx, EnterDoubleStrLen  ; string len
                syscall

                mov rdi, [ReadBufferLen]
                cmp [ReadBufferPos], rdi
                jb .bufIsNotEmpty

                mov rax, 0x00               ; sys_read
                mov rdi, 0x00               ; stdin
                lea rsi, [ReadBuffer]       ; buffer
                mov rdx, BufCapacity        ; buffer size
                syscall

                mov rdi, rax
                mov qword [ReadBufferLen], rdi
                mov qword [ReadBufferPos], 0

.bufIsNotEmpty:
                mov rsi, [ReadBufferPos]

                ; rdi  - buf len
                ; rsi  - buf pos
                ; rbx  - cur symbol
                ; r8   - is negative
                ; r9   - is after floating point
                ; r10  - exp10
                ; r11  = 10
                ; rax  - number

                xor r8, r8
                xor r9, r9
                xor r10, r10
                xor rbx, rbx
                xor rax, rax
                mov r11, 10

                cmp rsi, rdi ; pos >= len
                jae .error

                cmp byte ReadBuffer[rsi], '-'
                jne .isPositive
                mov r8, 1
                inc rsi
                inc qword [ReadBufferPos] ; is needed for empty input check

.isPositive:
                jmp .whileClause
.whileBody:
                mul r11
                lea rax, [rbx - '0']

                add r10, r9 ; inc exp10
                inc rsi

.whileClause:
                cmp rsi, rdi ; pos >= len
                jae .endBuf

                mov bl, ReadBuffer[rsi] ; bl - cur symbol

                cmp bl, '0'
                jb .notDigit

                cmp bl, '9'
                ja .notDigit

                jmp .whileBody

.notDigit:
                cmp bl, '.'
                je .floatingPoint

                cmp bl, 0x0a
                je .endLine

                jmp .error

.floatingPoint:
                test r9, r9
                jne .error ; several floating points in one number

                mov r9, 1
                inc rsi
                inc qword [ReadBufferPos] ; is needed for empty input check

                jmp .whileClause

.endLine:
                cmp [ReadBufferPos], rsi
                je .error ; empty input

                inc rsi
                cmp rsi, rdi
                je .endBuf

                mov qword [ReadBufferPos], rsi
                jmp .writeAns

.endBuf:
                cmp [ReadBufferPos], rsi
                je .error ; empty input

                mov qword [ReadBufferLen], 0
                mov qword [ReadBufferPos], 0

.writeAns:
                test r8, r8
                je .isPositive2

                neg rax

.isPositive2:
                cvtsi2sd xmm0, rax

                mov rax, 1

                jmp .forExpClause
.forExpBody:
                dec r10
                mul r11

.forExpClause:
                test r10, r10
                jne .forExpBody

                cvtsi2sd xmm1, rax

                divsd xmm0, xmm1

                ret

.error:
                mov rax, 0x01           ; sys_write
                mov rdi, 0x01           ; stdout
                lea rsi, [InputError]   ; string
                mov rdx, InputErrorLen  ; string len
                syscall

                mov rax, 0x3c           ; exit
                mov rdi, -1             ; error code
                syscall
;-------------------------------------------------

;-------------------------------------------------
; print double
;
; Args:     xmm0 - number
;
; Destr:    rax, rbx, rcx, rdx, rdi, rsi, r11, r12, r13, xmm0, xmm1
;-------------------------------------------------
doubleio_out:

                lea r13, [WriteBuffer + BufCapacity - 1]         ; r13 - output str pointer

                mov byte [r13], 0x0a ; \n
                dec r13

                movq rbx, xmm0                              ; rbx - raw xmm0
                test rbx, rbx
                jns .isPositive ; xmm0 >= 0

                movq xmm1, xmm0
                pxor xmm0, xmm0
                subsd xmm0, xmm1
.isPositive:

                cvttsd2si r11, xmm0 ; convert float to int  ; r11 - int part
                cvtsi2sd xmm1, r11
                subsd xmm0, xmm1
                mulsd xmm0, [double_1e6]
                cvttsd2si rax, xmm0                         ; rax - non-int part

                ; convert non-int part
                mov r12d, 10    ; base
                mov rcx, 6      ; width
.flWhileBody:
                xor rdx, rdx
                div r12d
                ; eax = div
                ; edx = mod

                mov dl, HexTable[rdx]
                mov byte [r13], dl
                dec r13
                dec rcx

                test rcx,rcx
                jne .flWhileBody

                mov byte [r13], '.'
                dec r13

                mov rax, r11                                ; rax - int part

                ; convert int part
.intWhileBody:
                xor rdx, rdx
                div r12d
                ; eax = div
                ; edx = mod

                mov dl, HexTable[rdx]
                mov byte [r13], dl
                dec r13

                test eax, eax
                jne .intWhileBody

                test rbx, rbx
                jns .isPositive2 ; xmm0 >= 0

                mov byte [r13], '-'
                dec r13
.isPositive2:

                mov r11, WriteBuffer + BufCapacity - 1
                sub r11, r13

                inc r13

                mov rax, 0x01   ; sys_write
                mov rdi, 0x01   ; stdout
                mov rsi, r13    ; buffer
                mov rdx, r11    ; buffer size
                syscall

                ret
;-------------------------------------------------

section .data

BufCapacity equ 64

WriteBuffer:   times BufCapacity db 0

ReadBuffer:     times BufCapacity db 0
ReadBufferLen:  dq 0
ReadBufferPos:  dq 0

section .rodata

HexTable:       db "0123456789abcdef"
double_1e6:     dq 0x412e848000000000

EnterDoubleStr:     db "Enter double: "
EnterDoubleStrLen   equ $ - EnterDoubleStr

InputError:         db "Wrong input. Exiting", 0x0a
InputErrorLen       equ $ - InputError

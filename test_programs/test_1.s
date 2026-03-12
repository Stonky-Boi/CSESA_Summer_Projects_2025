# Add Two Numbers

.data
num1: .word 5
num2: .word 3
result: .word 0

.text
main:
    LUI $t9, 4097          # 0x10010000

    LW $t0, 0($t9)         # num1
    LW $t1, 4($t9)         # num2

    ADD $t2, $t0, $t1      # t2 = num1 + num2
    SW $t2, 8($t9)         # result

    LW $a0, 8($t9)         # Load result into $a0 for print
    ADDI $v0, $zero, 1     # Syscall 1 = print integer
    SYSCALL

    ADDI $v0, $zero, 10    # Syscall 10 = exit
    SYSCALL
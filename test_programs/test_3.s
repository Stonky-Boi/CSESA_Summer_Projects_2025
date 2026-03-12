# Find the Larger of Two Numbers

.data
num1: .word 12
num2: .word 20
larger: .word 0

.text
main:
    LUI $t9, 4097          # 0x10010000

    LW $t0, 0($t9)         # num1
    LW $t1, 4($t9)         # num2

    SLT $t2, $t0, $t1      # t2 = 1 if num1 < num2
    BNE $t2, $zero, num2_larger

    SW $t0, 8($t9)         # larger = num1
    J print_result

num2_larger:
    SW $t1, 8($t9)         # larger = num2

print_result:
    LW $a0, 8($t9)         # Load result
    ADDI $v0, $zero, 1     # Syscall 1 = print integer
    SYSCALL

    ADDI $v0, $zero, 10    # Exit syscall
    SYSCALL
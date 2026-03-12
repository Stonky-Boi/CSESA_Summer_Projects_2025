# Reverse a String

.data
str: .asciiz "HELLO"
rev: .byte 0,0,0,0,0,0   # buffer for reversed string

.text
main:
    LUI $t9, 4097          # 0x10010000

    ADDI $t0, $t9, 0       # str pointer
    ADDI $t1, $t9, 6       # rev pointer
    ADDI $t2, $zero, 0     # length counter

find_len:
    LB $t3, 0($t0)
    BEQ $t3, $zero, reverse_loop
    ADDI $t0, $t0, 1
    ADDI $t2, $t2, 1
    J find_len

reverse_loop:
    ADDI $t0, $t0, -1      # move back one
    LB $t3, 0($t0)
    SB $t3, 0($t1)
    ADDI $t1, $t1, 1
    ADDI $t2, $t2, -1
    BNE $t2, $zero, reverse_loop

SB $zero, 0($t1)

ADDI $t1, $t9, 6           # pointer to rev
print_loop:
    LB $t3, 0($t1)
    BEQ $t3, $zero, end_print
    ADDI $a0, $t3, 0
    ADDI $v0, $zero, 11     # syscall 11 = print char
    SYSCALL
    ADDI $t1, $t1, 1
    J print_loop

end_print:
    ADDI $v0, $zero, 10      # exit
    SYSCALL
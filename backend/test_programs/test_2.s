# Print a String

.data
msg: .asciiz "Hello, MIPS!\n"

.text
main:
    LUI $t9, 4097          # 0x10010000

    ADDI $a0, $t9, 0       # msg is at offset 0

    ADDI $v0, $zero, 4     # Syscall 4 = print string
    SYSCALL

    ADDI $v0, $zero, 10    # Syscall 10 = exit
    SYSCALL
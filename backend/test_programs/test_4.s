# Loop to Add Numbers from 1 to 5

.data
sum: .word 0

.text
main:
    LUI $t9, 4097          # 0x10010000

    ADDI $t0, $zero, 1     # counter = 1
    ADDI $t1, $zero, 5     # upper limit = 5
    ADDI $t2, $zero, 0     # sum = 0

loop:
    ADD $t2, $t2, $t0      # sum += counter
    ADDI $t0, $t0, 1       # counter++

    SLT $t3, $t1, $t0         # t3 = 1 if t1 < t0  (counter exceeded limit)
    BNE $t3, $zero, end_loop  # if counter > t1, exit loop
    J loop

end_loop:
    SW $t2, 0($t9)         # sum at offset 0

    LW $a0, 0($t9)         # load sum into $a0
    ADDI $v0, $zero, 1     # syscall 1 = print integer
    SYSCALL

    ADDI $v0, $zero, 10    # syscall 10 = exit
    SYSCALL
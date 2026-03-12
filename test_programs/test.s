.text
main:
    ADDI $t0, $zero, 5      # counter = 5
    ADDI $t1, $zero, 0      # sum = 0
    ADDI $t2, $zero, 1      # decrement value = 1
loop:
    BEQ $t0, $zero, end     # if counter == 0, break loop
    ADD $t1, $t1, $t0       # sum = sum + counter
    SUB $t0, $t0, $t2       # counter = counter - 1
    J loop                  # jump back to loop
end:
    ADD $a0, $t1, $zero     # move sum to $a0 for printing
    ADDI $v0, $zero, 1      # syscall 1 (print integer)
    SYSCALL
    ADDI $v0, $zero, 10     # syscall 10 (exit program)
    SYSCALL
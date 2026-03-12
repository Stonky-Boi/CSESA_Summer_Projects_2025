.text
main:
    ADDI $t0, $zero, 10         # Outer loop counter (i = 10)
    ADDI $t2, $zero, 0          # Total sum = 0
    ADDI $t3, $zero, 1          # Constant 1 for subtraction

outer_loop:
    ADDI $t1, $zero, 20         # Inner loop counter (j = 20)
    
inner_loop:
    ADD $t2, $t2, $t3           # sum = sum + 1
    SUB $t1, $t1, $t3           # j = j - 1
    BNE $t1, $zero, inner_loop  # INNER BRANCH: If j != 0, repeat inner_loop
    
    SUB $t0, $t0, $t3           # i = i - 1
    BNE $t0, $zero, outer_loop  # OUTER BRANCH: If i != 0, repeat outer_loop
    
end:
    ADD $a0, $t2, $zero         # Move final sum (200) to $a0 for printing
    ADDI $v0, $zero, 1          # Syscall 1 (print integer)
    SYSCALL
    
    ADDI $v0, $zero, 10         # Syscall 10 (exit program)
    SYSCALL
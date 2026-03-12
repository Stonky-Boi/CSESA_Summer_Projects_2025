.data
fib0: .word 0
fib1: .word 1

.text
main:
    LUI $t9, 4097          # $t9 = 0x10010000 (Base address of data segment)
    ADDI $t8, $t9, 8       # $t8 = 0x10010008 (Pointer to the first empty memory slot)
    
    ADDI $t0, $zero, 6     # $t0 = 6 (We will generate 6 new Fibonacci numbers)
    ADDI $t4, $zero, 1     # $t4 = 1 (Constant for subtraction)

    LW $t1, 0($t9)         # $t1 = 0
    LW $t2, 4($t9)         # $t2 = 1

loop:
    BEQ $t0, $zero, end    # If counter == 0, exit loop
    
    ADD $t3, $t1, $t2      # $t3 = $t1 + $t2
    
    SW $t3, 0($t8)         # Store the new number into memory at address in $t8
    
    ADD $t1, $t2, $zero    # $t1 = $t2
    ADD $t2, $t3, $zero    # $t2 = $t3
    
    ADDI $t8, $t8, 4       # Move the memory pointer forward by 4 bytes (1 word)
    SUB $t0, $t0, $t4      # Decrement loop counter
    
    J loop                 # Repeat

end:
    ADDI $v0, $zero, 10    # Syscall 10 (exit program)
    SYSCALL
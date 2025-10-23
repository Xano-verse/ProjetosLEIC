    .data
A:  .word 1,2,3,4,5,6,7,8,9
    .word 10,11,12,13,14,15,16
B:  .word 11,22,33,44,55,66,77
    .word 88,99,111,122,133
    .word 144,155,166
C:  .word 0,0,0,0,0,0,0,0,0
    .word 0,0,0,0,0,0,0

    .text
main:
    addi s1, zero, 0   # i = 0
    addi s2, zero, 16  # value of N
    la s3, A           # base of A
    la s4, B           # base of B
    la s5, C           # base of C
    #addi a7, zero, 10
    
loop: 
	lw t1, 0(s3)    		# A[i]
    lw t2, 0(s4)    		# B[i]
    addi s1, s1, 2    		# i += 2
    mul t3, t2, t1    		# B[i] * A[i]
    add t3, t3, t1    		# + A[i]
    sw t3, 0(s5)      		# C[i]
    
    lw t4, 4(s3) 			# A[i+1]
    lw t5, 4(s4) 			# B[i+1]
    addi s3, s3, 8			# base A + 8
    mul t6, t5, t4 			# B[i+1] * A[i+1]
    add t6, t6, t4 			# + A[i+1]
    sw t6, 4(s5) 			# C[i+1]

    addi s4, s4, 8			# base B + 8
    addi s5, s5, 8			# base C + 8
    
    bne s1, s2, loop
   
end:
    addi a7, zero, 10
    ecall             # Exit (syscall)


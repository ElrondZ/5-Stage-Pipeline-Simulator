# Lab2 - Pipelined Test
# EX-EX Memory addu+sw
# Branch register is the same as rd of previous instruction
# 11/1/2021
# Yuanzhe Liu yl7897@nyu.edu

lw r1 r1 8 # r1 = mem[8] = 1 (element we want to find)  0
lw r2 r2 12 # r2 = mem[12] = 2                            1
lw r3 r3 16 # r3 = mem[16] = 4                          2
lw r4 r4 20 # r4 = mem[20] = 8                          3

# NOP
addu r31 r31 r31 #                                      4
addu r30 r30 r30 #                                      5
addu r29 r29 r29 #                                      6
addu r28 r28 r28 #                                      7

addu r5 r4 r3 # r5 = 12
addu r31 r31 r31 #  
sw r5 r4 16 # mem[24] = 12

# NOP
addu r27 r27 r27 #                                      4
addu r26 r26 r26 #                                      5
addu r25 r25 r25 #                                      6
addu r24 r24 r24 #                                      7

addu r6 r5 r5 # r6 = r5 + r5 = 24
addu r31 r31 r31 # 
sw r3 r6 4 # mem[r6+4] = mem[28] = 4
halt




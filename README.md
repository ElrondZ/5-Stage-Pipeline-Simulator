# **MIPS 5 Stage Pipeline Simulator**

In this Lab we implemented implement a cycle-accurate simulator for a 5-stage pipelined
MIPS processor in C++. The simulator supports a subset of the MIPS instruction set and should
model the execution of each instruction cycle by cycle.

# Statement

The MIPS program is provided to the simulator as a text file “imem.txt” file that is used to initialize the Instruction Memory. Each line of the file corresponds to a Byte stored in the Instruction Memory in binary format, with the first line at address 0, the next line at address 1 and so on. Four contiguous lines correspond to a whole instruction. Note that the words stored in memory are in “Big-Endian” format, meaning that the most significant byte is stored first.

The Data Memory is initialized using the “dmem.txt” file. The format of the stored words is the same as the Instruction Memory. As with the instruction memory, the data memory addresses also begin at 0 and increment by one in each line.

The instructions to be supported and their encodings are shown in Table 1. Note that all instructions, except for “halt”, exist in the MIPS ISA. The MIPS Green Sheet defines the semantics of each instruction.

For the purposes of this lab only, we will assume that the beq (branch-if-qual) instruction operates like a bne (branch-if-not-equal) instruction. In other words, in our implementations we assumed that the beq jumps to the branch address if 𝑅[𝑟𝑠] ≠ 𝑅[𝑟𝑡] and jumps to PC+4 otherwise, i.e., if 𝑅[𝑟𝑠] = 𝑅[𝑟𝑡]. (Note that a real beq instruction would operate in the opposite fashion, that is, it will jump to the branch address if 𝑅[𝑟𝑠] = 𝑅[𝑟𝑡] and to PC+4 otherwise. The reason we had to make this modification for this lab is because to implement loops we actually need the bne instruction.)

# 5 Stages

1. **Fetch (IF)**: fetches an instruction from instruction memory. Updates PC.
2. **Decode (ID/RF)**: reads from the register RF and generates control signals required in subsequent stages. In addition, branches are resolved in this stage by checking for the branch condition and computing the effective address.
3. **Execute (EX)**: performs an ALU operation.
4. **Memory (MEM)**: loads or stores a 32-bit word from data memory.
5. **Writeback (WB)**: writes back data to the RF.

# Dealing with Hazards

1. RAW Hazards: RAW hazards are dealt with using either only forwarding (if possible) or, if not, using stalling + forwarding.
2. Control Flow Hazards: We assumed that branch conditions are resolved in the ID/RF stage of the pipeline. Our processor deals with beq instructions as follows:
   1. Branches are always assumed to be NOT TAKEN. That is, when a beq is fetched in the IF stage, the PC is speculatively updated as PC+4.
   2. Branch conditions are resolved in the ID/RF stage. Every beq instruction has no RAW dependency with its previous two instructions.
   3. If the branch is NOT TAKEN, execution proceeds normally. However, if the branch is TAKEN, the speculatively fetched instruction from PC+4 is quashed in its ID/RF stage using the nop bit and the next instruction is fetched from the effective branch address. Execution now proceeds normally.

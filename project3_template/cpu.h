#ifndef CPU_H_
#define CPU_H_

/**
 * @brief definition of const variables
 * Note : You are free to edit this section according to your implementation
 */
#define NUM_REGISTERS 64
#define BYTE 8
#define MEM_SIZE 64*1024
#define CPU_COUNTERS 17

/**
 * @brief definition of the Instruction struc
 * Note : You are free to edit this function according to your implementation
 */
typedef struct {
    int length;    // length of the instruction
    int opcode;    // opcode
    int rd;    // destination register
    int rs1;    // source register 1
    int rs2;    // source register 2
    int has_rd;
    int has_rs1;
    int has_rs2;
    int imm1;    // immediate value 1
    int imm2;    // immediate value 2
    int has_imm1;    // has immediate value 1?
    int has_imm2;    // has immediate value 2?
} Instruction;


/**
 * @brief The definition of the CPU
 *  Note : You are free to edit this function according to your implementation
 */
// typedef struct CPU
// {
//     registers
//     stages (IF, Decode, Exe, Mem, WR, etc)
//     other informations needed to define here
// } CPU;

/**
 * @brief The definition of the CPU
 */
typedef struct CPU
{
    // 4 bytes integer registers
	int registers[NUM_REGISTERS];

	// CPU stats
    int stats[CPU_COUNTERS];

    // other informations needed to define here
    int memory[MEM_SIZE];

    // char array memout
    char memout[MEM_SIZE];

    Instruction* instructions;
} CPU_t;


#endif
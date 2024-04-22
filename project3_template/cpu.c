#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "cpu.h"

void print_registers(int* registers);
void print_statistics(int stats[]);

// variables & functions

int main(int argc, char const *argv[])
{
    // Step1: Load memory file
    FILE *fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        perror("Error loading memory");
        return 1;
    }

    // pipeline design of cpu simulation

    // print register values

    // print statistics

}

/**
 * @brief print the statistics of the instructions and CPU cycle
 * @param stats 
 * Note : You CAN make changes to variable names, but MUST NOT change the print format
 */
void print_statistics(int stats[]) {
    printf("\n\n------ Results ------\n");
    printf("Number of Set instructions: %d\n", stats[0]);
    printf("Number of Add instructions: %d\n", stats[1]);
    printf("Number of Sub instructions: %d\n", stats[2]);
    printf("Number of Mul instructions: %d\n", stats[3]);
    printf("Number of Div instructions: %d\n", stats[4]);
    printf("Number of LD instructions: %d\n", stats[5]);
    printf("Number of SD instructions: %d\n", stats[6]);
    printf("Number of BEZ instructions: %d\n", stats[7]);
    printf("Number of BGTZ instructions: %d\n", stats[8]);
    printf("Number of BLTZ instructions: %d\n", stats[9]);
    printf("Number of RET instructions: %d\n", stats[10]);
    printf("Number of 3Bytes instructions: %d\n", stats[11]);
    printf("Number of 4Bytes instructions: %d\n", stats[12]);
    printf("Number of 5Bytes instructions: %d\n", stats[13]);
    printf("Number of 6Bytes instructions: %d\n", stats[14]);
    printf("Total Number of instructions: %d\n", stats[15]);

    printf("\n");
    printf("Stalled cycles due to data hazard: %d \n", data_hazard_count);
    printf("Stalled cycles due to control hazard: %d \n", control_hazard_count);
    printf("Total execution cycles: %d\n", total_cycle_count);
    printf("IPC: %f\n", (total_instruction_count / (double)total_cycle_count));
}


/**
 * @brief print register vector
 * @param registers 
 *  Note : You are NOT supposed to edit this function
 */
void print_registers(int* registers) {
    printf("------ Registers ------\n");
    for (int i = 0; i < NUM_REGISTERS; ++i) {
        printf("[R%d]:%d ", i, registers[i]);
    }
    printf("\n");
}

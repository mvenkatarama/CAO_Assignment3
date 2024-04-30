#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "cpu.h"

/*
Logic to simulate pipeline stats:
1) Read an instraction
2) Read next 4 instructions
    2.1) if 1st inst has hazard, add 4 stalling cycle to data-hazards
    2.2) if 2nd inst has hazard, add 3 stalling cycle to data-hazards
    2.3) if 3rd inst has hazard, add 2 stalling cycle to data-hazards
    2.4) if 4th inst has hazard, add 1 stalling cycle to data-hazards
3) if control hazard in current inst, then add 4 cycles to control hazards 

*/
void print_registers(int* registers);
void print_statistics(int stats[]);

CPU_t cpu;

int data_hazard_count = 0, control_hazard_count = 0, total_cycle_count = 0, total_instruction_count = 0;

Instruction initInstruction(){
    Instruction instruction;
    instruction.length = -1;
    instruction.opcode = -1 ; 
    instruction.rd = -1;
    instruction.rs1 = -1;
    instruction.rs2 = -1;
    instruction.has_rd = -1;
    instruction.has_rs1 = -1;
    instruction.has_rs2 = -1;
    instruction.imm1 = -1;
    instruction.imm2 = -1;
    instruction.has_imm1 = -1;
    instruction.has_imm2 = -1;
    return instruction;
}

void setCPUIntsructionAtIndex(int index, int opcode, int rs1, int rs2, int rd, int imm1, int imm2){
    cpu.instructions[index].opcode = opcode;
    cpu.instructions[index].rs1 = rs1;
    cpu.instructions[index].rs2 = rs2;
    cpu.instructions[index].rd = rd;
    cpu.instructions[index].imm1 = imm1;
    cpu.instructions[index].imm2 = imm2;

}

int instructionCount = 0;

int readFile(const char *filePath) {
    FILE *binaryFile;
    unsigned int instructionBuffer[1];
    instructionBuffer[0] = 0;

    binaryFile = fopen(filePath, "rb");

    if (binaryFile == NULL) {
        perror("Error opening the file");
        return 1;
    }

    unsigned int rx=0, ry=0, rz=0, imm_value1=0, imm_value2=0, first_byte=0, second_byte=0, temp = 0, opcode = 0;
    int flag = 1; 
    
    
    // printf("\nCheckpoint 1\n");
    while (fread(instructionBuffer, 1, 1, binaryFile) == 1 && flag==1) {
        // printf("\nRead Instruction %d: 0x%02X", i, instructionBuffer[0]);
        int number_of_operands = 0;
        cpu.stats[16]+=1;
        cpu.stats[15]+=1;
        int pc = (int) ftell(binaryFile);
        switch (instructionBuffer[0])
        {
        
        // set - 4B
        case 0x01:
            opcode = instructionBuffer[0];

            fread(instructionBuffer, 1, 1, binaryFile);
            rx = instructionBuffer[0];

            fread(instructionBuffer, 1, 1, binaryFile);
            first_byte = instructionBuffer[0];
            second_byte = first_byte << 8;
            fread(instructionBuffer, 1, 1, binaryFile);
            second_byte += instructionBuffer[0];
            imm_value1 = second_byte;

            cpu.registers[rx] = imm_value1;
            cpu.stats[0]+=1;
            cpu.stats[12]+=1;

            setCPUIntsructionAtIndex(instructionCount, opcode, -1, -1, rx, imm_value1, -1);

            break;

        // add 4B
        case 0x10:
            opcode = instructionBuffer[0];

            // printf("\n%d - Add 1", cpu.stats[16]);
            fread(instructionBuffer, 1, 1, binaryFile);
            rx = instructionBuffer[0];
            fread(instructionBuffer, 1, 1, binaryFile);
            ry = instructionBuffer[0];
            fread(instructionBuffer, 1, 1, binaryFile);
            rz = instructionBuffer[0];
            cpu.registers[rx] = cpu.registers[ry] + cpu.registers[rz];

            cpu.stats[1]+=1;
            cpu.stats[12]+=1;

            setCPUIntsructionAtIndex(instructionCount, opcode, ry, rz, rx, -1, -1);

            break;

        // add 5B
        case 0x50:
            opcode = instructionBuffer[0];

            // printf("\n%d - Add 2", cpu.stats[16]);
            fread(instructionBuffer, 1, 1, binaryFile);
            rx = instructionBuffer[0];
            fread(instructionBuffer, 1, 1, binaryFile);
            ry = instructionBuffer[0];

            fread(instructionBuffer, 1, 1, binaryFile);
            first_byte = instructionBuffer[0];
            second_byte = first_byte << 8;
            fread(instructionBuffer, 1, 1, binaryFile);
            second_byte += instructionBuffer[0];
            imm_value1 = second_byte;

            cpu.registers[rx] = cpu.registers[ry] + imm_value1;
            cpu.stats[1]+=1;
            cpu.stats[13]+=1;

            setCPUIntsructionAtIndex(instructionCount, opcode, ry, -1, rx, imm_value1, -1);

            break;

        // add 6B
        case 0x90:
            opcode = instructionBuffer[0];
            
            // printf("\n%d - Add 3",cpu.stats[16]);
            fread(instructionBuffer, 1, 1, binaryFile);
            rx = instructionBuffer[0];

            fread(instructionBuffer, 1, 1, binaryFile);
            first_byte = instructionBuffer[0];
            second_byte = first_byte << 8;
            fread(instructionBuffer, 1, 1, binaryFile);
            second_byte += instructionBuffer[0];
            imm_value1 = second_byte;

            fread(instructionBuffer, 1, 1, binaryFile);
            first_byte = instructionBuffer[0];
            second_byte = first_byte << 8;
            fread(instructionBuffer, 1, 1, binaryFile);
            second_byte += instructionBuffer[0];
            imm_value2 = second_byte;

            cpu.registers[rx] = imm_value1 + imm_value2;
            cpu.stats[1]+=1;
            cpu.stats[14]+=1;

            setCPUIntsructionAtIndex(instructionCount, opcode, -1, -1, rx, imm_value1, imm_value2);

            break;

        // sub 4B
        case 0x14:
            opcode = instructionBuffer[0];
            
            // printf("\n%d - Sub 1", cpu.stats[16]);
            fread(instructionBuffer, 1, 1, binaryFile);
            rx = instructionBuffer[0];
            fread(instructionBuffer, 1, 1, binaryFile);
            ry = instructionBuffer[0];
            fread(instructionBuffer, 1, 1, binaryFile);
            rz = instructionBuffer[0];
            cpu.registers[rx] = cpu.registers[ry] - cpu.registers[rz];
            cpu.stats[2]+=1;
            cpu.stats[12]+=1;

            setCPUIntsructionAtIndex(instructionCount, opcode, ry, rz, rx, -1, -1);

            break;

        // sub 5B
        case 0x54:
            opcode = instructionBuffer[0];
            
            // printf("\n%d - Sub 2",cpu.stats[16]);
            fread(instructionBuffer, 1, 1, binaryFile);
            rx = instructionBuffer[0];
            fread(instructionBuffer, 1, 1, binaryFile);
            ry = instructionBuffer[0];
            
            fread(instructionBuffer, 1, 1, binaryFile);
            first_byte = instructionBuffer[0];
            second_byte = first_byte << 8;
            fread(instructionBuffer, 1, 1, binaryFile);
            second_byte += instructionBuffer[0];
            imm_value1 = second_byte;

            cpu.registers[rx] = cpu.registers[ry] - imm_value1;
            cpu.stats[2]+=1;
            cpu.stats[13]+=1;

            setCPUIntsructionAtIndex(instructionCount, opcode, ry, -1, rx, imm_value1, -1);

            break;

        // sub 6B
        case 0x94:
            opcode = instructionBuffer[0];
            
            // printf("\n%d - Sub 3", cpu.stats[16]);
            fread(instructionBuffer, 1, 1, binaryFile);
            rx = instructionBuffer[0];

            fread(instructionBuffer, 1, 1, binaryFile);
            first_byte = instructionBuffer[0];
            second_byte = first_byte << 8;
            fread(instructionBuffer, 1, 1, binaryFile);
            second_byte += instructionBuffer[0];
            imm_value1 = second_byte;

            fread(instructionBuffer, 1, 1, binaryFile);
            first_byte = instructionBuffer[0];
            second_byte = first_byte << 8;
            fread(instructionBuffer, 1, 1, binaryFile);
            second_byte += instructionBuffer[0];
            imm_value2 = second_byte;

            cpu.registers[rx] = imm_value1 - imm_value2;
            cpu.stats[2]+=1;
            cpu.stats[14]+=1;

            setCPUIntsructionAtIndex(instructionCount, opcode, -1, -1, rx, imm_value1, imm_value2);

            break;

        // mul 4B
        case 0x18:
            opcode = instructionBuffer[0];
            
            // printf("\n%d - Mul 1",cpu.stats[16]);
            fread(instructionBuffer, 1, 1, binaryFile);
            rx = instructionBuffer[0];
            fread(instructionBuffer, 1, 1, binaryFile);
            ry = instructionBuffer[0];
            fread(instructionBuffer, 1, 1, binaryFile);
            rz = instructionBuffer[0];
            cpu.registers[rx] = cpu.registers[ry] * cpu.registers[rz];

            cpu.stats[3]+=1;
            cpu.stats[12]+=1;

            setCPUIntsructionAtIndex(instructionCount, opcode, ry, rz, rx, -1, -1);

            break;

        // mul 5B
        case 0x58:
            opcode = instructionBuffer[0];
            
            // printf("\n%d - Mul 2", cpu.stats[16]);
            fread(instructionBuffer, 1, 1, binaryFile);
            rx = instructionBuffer[0];
            fread(instructionBuffer, 1, 1, binaryFile);
            ry = instructionBuffer[0];

            fread(instructionBuffer, 1, 1, binaryFile);
            first_byte = instructionBuffer[0];
            second_byte = first_byte << 8;
            fread(instructionBuffer, 1, 1, binaryFile);
            second_byte += instructionBuffer[0];
            imm_value1 = second_byte;

            cpu.registers[rx] = cpu.registers[ry] * imm_value1;
            
            cpu.stats[3]+=1;
            cpu.stats[13]+=1;

            setCPUIntsructionAtIndex(instructionCount, opcode, ry, -1, rx, imm_value1, -1);

            break;
        
        // mul 6B
        case 0x98:
            opcode = instructionBuffer[0];
           
            // printf("\n%d - Mul 3", cpu.stats[16]);
            fread(instructionBuffer, 1, 1, binaryFile);
            rx = instructionBuffer[0];

            fread(instructionBuffer, 1, 1, binaryFile);
            first_byte = instructionBuffer[0];
            second_byte = first_byte << 8;
            fread(instructionBuffer, 1, 1, binaryFile);
            second_byte += instructionBuffer[0];
            imm_value1 = second_byte;

            fread(instructionBuffer, 1, 1, binaryFile);
            first_byte = instructionBuffer[0];
            second_byte = first_byte << 8;
            fread(instructionBuffer, 1, 1, binaryFile);
            second_byte += instructionBuffer[0];
            imm_value2 = second_byte;

            cpu.registers[rx] = imm_value1 * imm_value2;

            cpu.stats[3]+=1;
            cpu.stats[14]+=1;

            setCPUIntsructionAtIndex(instructionCount, opcode, -1, -1, rx, imm_value1, imm_value2);

            break;

        // div 4B
        case 0x1C:
            opcode = instructionBuffer[0];
            
            // printf("\n%d - Div 1", cpu.stats[16]);
            fread(instructionBuffer, 1, 1, binaryFile);
            rx = instructionBuffer[0];
            fread(instructionBuffer, 1, 1, binaryFile);
            ry = instructionBuffer[0];
            fread(instructionBuffer, 1, 1, binaryFile);
            rz = instructionBuffer[0];

            cpu.registers[rx] = cpu.registers[ry] / cpu.registers[rz];
            
            cpu.stats[4]+=1;
            cpu.stats[12]+=1;

            setCPUIntsructionAtIndex(instructionCount, opcode, ry, rz, rx, -1, -1);

            break;

        // div 5B
        case 0x5C:
            opcode = instructionBuffer[0];
            
            // printf("\n%d - Div 2", cpu.stats[16]);
            fread(instructionBuffer, 1, 1, binaryFile);
            rx = instructionBuffer[0];
            fread(instructionBuffer, 1, 1, binaryFile);
            ry = instructionBuffer[0];

            fread(instructionBuffer, 1, 1, binaryFile);
            first_byte = instructionBuffer[0];
            second_byte = first_byte << 8;
            fread(instructionBuffer, 1, 1, binaryFile);
            second_byte += instructionBuffer[0];
            imm_value1 = second_byte;

            cpu.registers[rx] = cpu.registers[ry] / imm_value1;

            cpu.stats[4]+=1;
            cpu.stats[13]+=1;

            setCPUIntsructionAtIndex(instructionCount, opcode, ry, -1, rx, imm_value1, -1);

            break;
        
        // div 6B
        case 0x9C:
            opcode = instructionBuffer[0];
            
            // printf("\n%d - Div 3", cpu.stats[16]);
            fread(instructionBuffer, 1, 1, binaryFile);
            rx = instructionBuffer[0];

            fread(instructionBuffer, 1, 1, binaryFile);
            first_byte = instructionBuffer[0];
            second_byte = first_byte << 8;
            fread(instructionBuffer, 1, 1, binaryFile);
            second_byte += instructionBuffer[0];
            imm_value1 = second_byte;

            fread(instructionBuffer, 1, 1, binaryFile);
            first_byte = instructionBuffer[0];
            second_byte = first_byte << 8;
            fread(instructionBuffer, 1, 1, binaryFile);
            second_byte += instructionBuffer[0];
            imm_value2 = second_byte;

            cpu.registers[rx] = imm_value1 / imm_value2;
            cpu.stats[4]+=1;
            cpu.stats[14]+=1;

            setCPUIntsructionAtIndex(instructionCount, opcode, -1, -1, rx, imm_value1, imm_value2);

            break;

        // BEZ - 4B
        case 0x04:
            opcode = instructionBuffer[0];
            
            fread(instructionBuffer, 1, 1, binaryFile);
            rx = instructionBuffer[0];

            fread(instructionBuffer, 1, 1, binaryFile);
            first_byte = instructionBuffer[0];
            second_byte = first_byte << 8;
            fread(instructionBuffer, 1, 1, binaryFile);
            second_byte += instructionBuffer[0];
            imm_value1 = second_byte;

            if(cpu.registers[rx]==0) {
                fseek(binaryFile, imm_value1, SEEK_SET);
                // long seek_pos = ftell(binaryFile);
                // seek_pos = ftell(binaryFile);
                // printf(" BEZ-After seek position:%ld, imm_val: %d, rx: %d, cpu.registers[rx]:%d \t", seek_pos, imm_value1, rx, cpu.registers[rx]);
                // print_registers(cpu.registers);
            }

            cpu.stats[7]+=1;
            cpu.stats[12]+=1;

            setCPUIntsructionAtIndex(instructionCount, opcode, rx, -1, rx, imm_value1, -1);

            break;
        
        // BGTZ - 4B
        case 0x05:
            opcode = instructionBuffer[0];
            
            fread(instructionBuffer, 1, 1, binaryFile);
            rx = instructionBuffer[0];

            fread(instructionBuffer, 1, 1, binaryFile);
            first_byte = instructionBuffer[0];
            second_byte = first_byte << 8;
            fread(instructionBuffer, 1, 1, binaryFile);
            second_byte += instructionBuffer[0];
            imm_value1 = second_byte;

            if(cpu.registers[rx]>0){
                // printf("BGTZ-Before seek position:%ld\t", seek_pos);
                fseek(binaryFile, imm_value1, SEEK_SET);
                // long seek_pos = ftell(binaryFile);
                // seek_pos = ftell(binaryFile);
                // printf(" BGTZ-After seek position:%ld\t", seek_pos);
            } 

            cpu.stats[8]+=1;
            cpu.stats[12]+=1;

            setCPUIntsructionAtIndex(instructionCount, opcode, rx, -1, rx, imm_value1, -1);

            break;

        // BLTZ - 4B
        case 0x06:
            opcode = instructionBuffer[0];
            
            fread(instructionBuffer, 1, 1, binaryFile);
            rx = instructionBuffer[0];

            fread(instructionBuffer, 1, 1, binaryFile);
            first_byte = instructionBuffer[0];
            second_byte = first_byte << 8;
            fread(instructionBuffer, 1, 1, binaryFile);
            second_byte += instructionBuffer[0];
            imm_value1 = second_byte;

            // printf("\nBLTZ: rx: %d, imm_value1:%d", rx, imm_value1);
            if(cpu.registers[rx]<0){
                // printf("BLTZ-Before seek position:%ld\t", seek_pos);
                fseek(binaryFile, imm_value1, SEEK_SET);
                // long seek_pos = ftell(binaryFile);
                // seek_pos = ftell(binaryFile);
                // printf(" BLTZ-After seek position:%ld\t", seek_pos);
            } 

            cpu.stats[9]+=1;
            cpu.stats[12]+=1;

            setCPUIntsructionAtIndex(instructionCount, opcode, rx, -1, rx, imm_value1, -1);

            break;

        // LD - 3B
        case 0xC8:
            opcode = instructionBuffer[0];
            
            fread(instructionBuffer, 1, 1, binaryFile);
            rx = instructionBuffer[0];
            fread(instructionBuffer, 1, 1, binaryFile);
            ry = instructionBuffer[0];

            // printf("\nLD: rx: %d, ry:%d", rx, ry);
            cpu.registers[rx] = cpu.memory[cpu.registers[ry]];

            cpu.stats[5]+=1;
            cpu.stats[11]+=1;

            setCPUIntsructionAtIndex(instructionCount, opcode, ry, -1, rx, -1, -1);
        
            break;

        // SD - 3B
        case 0xCC:
            opcode = instructionBuffer[0];
            
            fread(instructionBuffer, 1, 1, binaryFile);
            rx = instructionBuffer[0];
            fread(instructionBuffer, 1, 1, binaryFile);
            ry = instructionBuffer[0];

            // printf("\nSD: rx: %d, ry:%d", rx, ry);
            cpu.memory[cpu.registers[ry]] = cpu.registers[rx];

            int value = cpu.registers[rx];

            cpu.memout[cpu.registers[ry]] = value;
            cpu.memout[cpu.registers[ry]+1] = value >> 1*8;
            cpu.memout[cpu.registers[ry]+2] = value >> 2*8;
            cpu.memout[cpu.registers[ry]+3] = value >> 3*8;

            cpu.stats[6]+=1;
            cpu.stats[11]+=1;

            setCPUIntsructionAtIndex(instructionCount, opcode, ry, -1, rx, -1, -1);

            break;


        case 0x00:
            opcode = instructionBuffer[0];
            
            // printf(" - Ret");
            cpu.stats[10]+=1;
            cpu.stats[12]+=1;
            flag = 0;

            setCPUIntsructionAtIndex(instructionCount, opcode, -1, -1, -1, -1, -1);

            break;
        
        default:
            // printf("\nInvalid @ %ld",ftell(binaryFile));
            break;
        }

        instructionCount+=1;
        
        // iteration-=1;
        // long pos = ftell(binaryFile);
        // printf(" POS : %ld\n", pos);
        // print_registers(cpu.registers);
    }

    // Close the file when done
    fclose(binaryFile);

    return 0;
}

char* getOpcodeInString(int opcode){
    // printf("In getOpcodeInString %d", opcode);
    if(opcode == -1 ){
        return "NoOp";
    }
    if(opcode!=-1){
        switch(opcode){
            case 0x01: return "set";
            case 0x10: return "add1";
            case 0x50: return "add2";
            case 0x90: return "add3";
            case 0x14: return "sub1";
            case 0x54: return "sub2";
            case 0x94: return "sub3";
            case 0x18: return "mul1";
            case 0x58: return "mul2";
            case 0x98: return "mul3";
            case 0x1C: return "div1";
            case 0x5C: return "div2";
            case 0x9C: return "div3";
            case 0x04: return "bez";
            case 0x05: return "bgtz";
            case 0x06: return "bltz";
            case 0xC8: return "ld";
            case 0xCC: return "sd";
            case 0x00: return "ret";
            default: {
                printf("\nWeird Opcode : %d", opcode);
                return "deafult";
            }        
        }
    }

}

void writeMemFile(const char *filePath) {

    // Calculate the number of elements in the array
    // int num_elements = sizeof(cpu.memory) / sizeof(cpu.memory[0]);

    // Open the binary file for writing
    FILE *file = fopen(filePath, "wb");
    if (file == NULL) {
        perror("Error opening file");
        fclose(file);
        return;
    }

    // Write the array elements to the file
    fwrite(cpu.memout, sizeof(char), MEM_SIZE, file);

    // Close the file
    fclose(file);

}

void loadMemFileIntoArr(const char *filePath){
    FILE *file;
    size_t bytes_read; 


    file = fopen(filePath, "rb");
    if (file == NULL) {
        perror("Error opening file");
        fclose(file);
        return;
    }


    bytes_read = fread(cpu.memout, sizeof(char), sizeof(cpu.memout), file);
    if (bytes_read == 0) {
        perror("Error reading from file");
        fclose(file);
        return;
    }

    // Close the file
    fclose(file);


    return;
}

void printOpcodes(){
    for(int i=0; i<100; i++){
        printf("Opcode at %d is %d - dest:%d\n", i, cpu.instructions[i].opcode, cpu.instructions[i].rd);
    }
}

void printPipeline(Instruction pipeline[]){
    // printf("\n");
    for(int i=7; i>=0; i--){
        printf("\nP[%d]: opcode : %s, R%d,R%d,R%d, imm1:%d, imm2:%d  ", i, getOpcodeInString(pipeline[i].opcode),pipeline[i].rd, pipeline[i].rs1,pipeline[i].rs2, pipeline[i].imm1, pipeline[i].imm2);
    }
    printf("\n");
}

Instruction* handleHazards(Instruction * pipeline){
    
    // control hazards
    Instruction instAtBR = pipeline[4];
    bool isControlHazard = false;

    printf("\nInst at BR: opcode:%d, rx:%d, rs1:%d, rs2:%d, imm1:%d, imm2:%d, control_hazard_count:%d \n", 
    instAtBR.opcode, instAtBR.rd, instAtBR.rs1, instAtBR.rs2, instAtBR.imm1, instAtBR.imm2, control_hazard_count);

    if(instAtBR.opcode == 0x04) { //BEZ
        if(cpu.registers[instAtBR.rd]==0) {
            control_hazard_count+=4;
            isControlHazard = true;
            printf("\ndue to BEZ\n");
        }

    } else if(instAtBR.opcode == 0x05) { // BGTZ
        if(cpu.registers[instAtBR.rd]>0) {
            control_hazard_count+=4;
            isControlHazard = true;
            printf("\ndue to BGTZ\n");
        } 

    } else if(instAtBR.opcode == 0x06) { // BLTZ
        if(cpu.registers[instAtBR.rd]<0) {
            control_hazard_count+=4;
            isControlHazard = true;
            printf("\ndue to BLTZ\n");
        }
    }

    // if(isControlHazard) {
    //     for(int i=0; i<4; i++) {
    //         pipeline[i] = initInstruction();
    //     }

    //     return pipeline;
    // }
 

    // data hazards
    Instruction instAtRR = pipeline[2];
    int i=0;
    if(instAtRR.opcode == 0x01) {
        return pipeline;
    }
    if(instAtRR.opcode == 0xC8 || instAtRR.opcode == 0xCC  ){ // ld || instAtRR.opcode == 0xCC
        for(i=3; i<8; i++){
            if(
                (instAtRR.rs1 != -1 && instAtRR.rs1 == pipeline[i].rd)
                || (instAtRR.rs2 != -1 && instAtRR.rs2 == pipeline[i].rd)  
                || (instAtRR.rd != -1 && instAtRR.rd == pipeline[i].rd)
            ) { printf("** %d **", pipeline[i].opcode);
                if(instAtRR.opcode == 0xC8 && pipeline[i].opcode == 0xCC){ // (pipeline[i].opcode==0xC8 &&)
                    continue;
                } else {
                    printf("D-hazard ");
                    break; 
                }   
            }
        }
    } 
    // else if(instAtRR.opcode == 0xCC ){ // sd || instAtRR.opcode == 0xCC
    //     for(i=3; i<8; i++){
    //         if(
    //             (instAtRR.rd != -1 && (instAtRR.rd == pipeline[i].rs1 || instAtRR.rd == pipeline[i].rs2 || instAtRR.rd == pipeline[i].rd))
    //         ) {
    //             printf("D-hazard ");
    //             break;   
    //         }
    //     }
    // } 
    // else if(instAtRR.opcode == 0x10){
    //     for(i=3; i<8; i++){
    //         // data hazard
    //         if(
    //             (instAtRR.rd != -1 && (instAtRR.rd == pipeline[i].rs1 || instAtRR.rd == pipeline[i].rs2))
    //         ) {
    //             printf("D-hazard ");
    //             break;   
    //         }
    //     }

    // } 
    else {
        for(i=3; i<8; i++){
            // data hazard
            if(
                (instAtRR.rs1 != -1 && (instAtRR.rs1 == pipeline[i].rd))
                || (instAtRR.rs2 != -1 && (instAtRR.rs2 == pipeline[i].rd))
                // (instAtRR.rs1 != -1 && (instAtRR.rs1 == pipeline[i].rs1 || instAtRR.rs1 == pipeline[i].rs2 || instAtRR.rs1 == pipeline[i].rd))
                // || (instAtRR.rs2 != -1 && (instAtRR.rs2 == pipeline[i].rs1 || instAtRR.rs2 == pipeline[i].rs2 || instAtRR.rs2 == pipeline[i].rd))
                // || (instAtRR.rd != -1 && (instAtRR.rd == pipeline[i].rs1 || instAtRR.rd == pipeline[i].rs2 || instAtRR.rd == pipeline[i].rd))
            ) {
                if(pipeline[i].opcode == 0xCC){
                    continue;
                } else {
                    printf("D-hazard ");
                    break; 
                }
                  
            }
        }
    }
    

    int stall = 0;
    if(i==8) {
        // no hazard 
        stall = 0;
    } else if(i==7) {
        // stall 1 cycle
        stall = 1;
    } else if(i==6) {
        // stall 2 cycle
        stall = 2;
    }else if(i==5) { 
        // stall 3 cycle
        stall = 3;
    }else if(i==4) {
        // stall 4 cycle
        stall = 4;
    }else if(i==3) {
        // stall 5 cycle
        stall = 5;
    }

    data_hazard_count += stall;
    printf(" stall :%d, data_hazard_count: %d, control_hazard:%d",
    stall, data_hazard_count, control_hazard_count);

    int nullIndex = 3;
    while(stall > 0) {
        for(int j=7; j>=4; j--) {
            pipeline[j] = pipeline[j-1];
            //case 0x04: return "bez";
            // case 0x05: return "bgtz";
            // case 0x06: return "bglz";
            if(j==4){ // Branch statement in BR stage
                if((pipeline[j].opcode == 0x04 && cpu.registers[pipeline[j].rd] == 0)
                || (pipeline[j].opcode == 0x05 && cpu.registers[pipeline[j].rd] > 0) 
                || (pipeline[j].opcode == 0x05 && cpu.registers[pipeline[j].rd] > 0)) {
                    control_hazard_count+=3;
                }
            }
        }
        pipeline[nullIndex] = initInstruction();
        nullIndex+=1;
        stall-=1;
    };

    return pipeline;

}

void runPipeline(){

    // [' IF:0', ' ID:1', ' RR:2', ' EX:3', ' BR:4', ' MEM1:5', ' MEM2:6', 'WR:7']
    Instruction * pipeline = malloc(8 * sizeof(Instruction));
    int count = 0;

    for(int i=0; i<8; i++) {
        pipeline[i] = initInstruction();
    }

    while(count <= instructionCount){
        printf("\nIteration: %d\n", count);
        // printPipeline(pipeline);

        handleHazards(pipeline);

        printPipeline(pipeline);
        for(int i=7; i>0; i--){
            pipeline[i] = pipeline[i-1];
        }
        pipeline[0] = cpu.instructions[count];
        

        // printPipeline(pipeline);
        count+=1;
    }

}


int main(int argc, char const *argv[])
{
	// CPU_t cpu;
	// Step1: initialization registers in the CPU, as well as other essential variables
	// initialize registers
    for (int i = 0; i < NUM_REGISTERS; ++i) {
        cpu.registers[i] = 0;
    }

	// initialize stats
    for (int i = 0; i < CPU_COUNTERS; ++i) {
        cpu.stats[i] = 0;
    }

    // initialize memory
    for(int i=0; i<MEM_SIZE; i++) {
        cpu.memory[i] = 0;
    }

    // initilaize instructions array
    cpu.instructions = malloc(1000 * sizeof(Instruction));
    for(int i=0; i<1000; i++) {
        cpu.instructions[i] = initInstruction();
    }

    // printf("%d\n", argc);
    // printf("%s\n", argv[0]);
    // printf("%s\n", argv[1]);

    loadMemFileIntoArr(argv[1]);

    readFile(argv[1]);
    // printOpcodes();

    writeMemFile(argv[2]);
    runPipeline();

    print_registers(cpu.registers);
	print_statistics(cpu.stats);
    // writeMemOutFile(argv[1], argv[2]);

	// Step2: read bits from the text area of memory

    // Step3: decode bits into instruction

    // Step4: execute the instruction

    // continue until the last instruction is executed or a Ret instruction is met, print the output, register info and the statistics required
    printf("\n");
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

    total_cycle_count = stats[15]+data_hazard_count+14;
    printf("\n");
    printf("Stalled cycles due to data hazard: %d \n", data_hazard_count);
    printf("Stalled cycles due to control hazard: %d \n", control_hazard_count);
    printf("Total execution cycles: %d\n", total_cycle_count);
    printf("IPC: %f\n", (stats[15] / (double)total_cycle_count));
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

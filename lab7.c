/*
 ============================================================================
 Name        : lab7.c
 Author      : Austin Tian
 Revised by  : Solomon Ewusi | Student ID: N01659373
 Version     :
 Copyright   : Copyright 2023
 Description : Lab 7 - MIPS Instruction Set Assembler in C.
               Converts MIPS assembly instructions into 32-bit machine code
               and loads them into the simulated memory space.
 ============================================================================
 */
#include "header.h"

// ---------------------------------------------------------------
// setupDataMemory: Copies the data section bytes into memory starting at offset.
// base_address = pointer to the start of the 1MB memory
// offset       = where in memory the data section starts (0x2000)
// datasection  = array of bytes from the .data section of the ASM file
// numberOfBytes= how many bytes to copy
void setupDataMemory(char* base_address,
                     unsigned int offset,
                     char *datasection,
                     unsigned int numberOfBytes){
    // Copy each byte from the data section array into memory at base+offset+i.
    // We use write_dword for every 4 bytes to stay consistent with memory functions.
    unsigned int i;
    for (i = 0; i < numberOfBytes; i++){
        // Cast to avoid sign issues when writing individual bytes
        *(unsigned char*)(base_address + offset + i) = (unsigned char)datasection[i];
    }
    return;
}

// ---------------------------------------------------------------
// buildIInstruction: Builds a 32-bit I-type MIPS machine code word.
// I-type format:  oooooo ss sss ttttt iiiiiiii iiiiiiii
//                 [31:26] [25:21] [20:16] [15:0]
// opcode    = 6-bit operation code
// rs        = 5-bit source register
// rt        = 5-bit target register
// immediate = 16-bit signed immediate value
unsigned int buildIInstruction(unsigned char opcode,
                               unsigned char rs,
                               unsigned char rt,
                               int immediate){
    unsigned int machineCode = 0;
    unsigned int mask = 0;
    // Place the 16-bit immediate in bits [15:0]
    machineCode = immediate & 0x0000FFFF;
    // Place rs in bits [25:21]
    mask = ((unsigned int)(rs & 0x000000FF)) << 21;
    machineCode = machineCode | mask;
    // Place rt in bits [20:16]
    mask = ((unsigned int)(rt & 0x000000FF)) << 16;
    machineCode = machineCode | mask;
    // Place opcode in bits [31:26]
    mask = ((unsigned int)(opcode & 0x000000FF)) << 26;
    machineCode = machineCode | mask;
    return machineCode;  // return the completed 32-bit I-type machine code
}

// ---------------------------------------------------------------
// buildJInstruction: Builds a 32-bit J-type MIPS machine code word.
// J-type format:  oooooo iiiiiiii iiiiiiii iiiiiiii iiiiiiii
//                 [31:26] [25:0]
// opcode    = 6-bit operation code
// immediate = 26-bit jump target address
unsigned int buildJInstruction(unsigned char opcode,
                               int immediate){
    unsigned int machineCode = 0;
    unsigned int mask = 0;
    // Place the 26-bit jump address in bits [25:0]
    machineCode = immediate & 0x03FFFFFF;
    // Place opcode in bits [31:26]
    mask = ((unsigned int)(opcode & 0x000000FF)) << 26;
    machineCode = machineCode | mask;
    return machineCode;  // return the completed 32-bit J-type machine code
}

// ---------------------------------------------------------------
// buildRInstruction: Builds a 32-bit R-type MIPS machine code word.
// R-type format:  oooooo sssss ttttt ddddd aaaaa ffffff
//                 [31:26] [25:21] [20:16] [15:11] [10:6] [5:0]
// opcode   = 6-bit operation code (always 0 for R-type)
// rs       = 5-bit source register 1
// rt       = 5-bit source register 2
// rd       = 5-bit destination register
// shamt    = 5-bit shift amount
// function = 6-bit function code (identifies the specific R-type instruction)
unsigned int buildRInstruction(unsigned char opcode,
                               unsigned char rs,
                               unsigned char rt,
                               unsigned char rd,
                               unsigned char shamt,
                               unsigned char function){
    unsigned int machineCode = 0;
    unsigned int mask = 0;
    // Place the 6-bit function code in bits [5:0]
    machineCode = function & 0x0000003F;
    // Place shamt in bits [10:6]
    mask = ((unsigned int)(shamt & 0x0000001F)) << 6;
    machineCode = machineCode | mask;
    // Place rd in bits [15:11]
    mask = ((unsigned int)(rd & 0x0000001F)) << 11;
    machineCode = machineCode | mask;
    // Place rt in bits [20:16]
    mask = ((unsigned int)(rt & 0x0000001F)) << 16;
    machineCode = machineCode | mask;
    // Place rs in bits [25:21]
    mask = ((unsigned int)(rs & 0x0000001F)) << 21;
    machineCode = machineCode | mask;
    // Place opcode in bits [31:26] (always 0x00 for R-type)
    mask = ((unsigned int)(opcode & 0x0000003F)) << 26;
    machineCode = machineCode | mask;
    return machineCode;  // return the completed 32-bit R-type machine code
}

// ---------------------------------------------------------------
// setupInstructionMemory: Iterates through Instruction_storage[], converts
// each MIPS instruction to its 32-bit machine code, and writes it to memory.
void setupInstructionMemory(char* base_memory_address,
                            int codeOffset,
                            MIPS_Instruction *instructionStorage){
    int i = 0;
    unsigned int totalinstruction = 0;
    unsigned char opcode = 0;
    unsigned int machineCode = 0;

    do {
        // ---- la instruction ----
        if (strcmp(instructionStorage[i].instruction, "la") == 0){
        // Q1: What type of instruction is this? J, R or I instruction?
        // Answer: I-type instruction
        // Q2: What is the machine language data format?
        // Answer: oooooo ss sss ttttt iiiiiiii iiiiiiii
        // needs: opcode, rs, rt, and immediate (address of the variable)
        // Q3: What is the opcode of this instruction?
        // Answer: 0b101111 (0x2F) - load address pseudo-instruction uses opcode 47
            opcode = 0b101111;
            machineCode = buildIInstruction(opcode,
                                            instructionStorage[i].rs,
                                            instructionStorage[i].rt,
                                            instructionStorage[i].immediate);
            write_dword(base_memory_address, codeOffset + i * 4, machineCode);
        }
        // ---- lb instruction ----
        else if (strcmp(instructionStorage[i].instruction, "lb") == 0){
        // Q1: What type of instruction is this? J, R or I instruction?
        // Answer: I-type instruction
        // Q2: What is the machine language data format?
        // Answer: oooooo ss sss ttttt iiiiiiii iiiiiiii
        // needs: opcode, rs (base register), rt (destination), immediate (offset)
        // Q3: What is the opcode of this instruction?
        // Answer: 0b100000 (0x20) - load byte opcode = 32
            opcode = 0b100000;
            machineCode = buildIInstruction(opcode,
                                            instructionStorage[i].rs,
                                            instructionStorage[i].rt,
                                            instructionStorage[i].immediate);
            write_dword(base_memory_address, codeOffset + i * 4, machineCode);
        }
        // ---- bge instruction ----
        else if (strcmp(instructionStorage[i].instruction, "bge") == 0){
        // Q1: What type of instruction is this? J, R or I instruction?
        // Answer: I-type instruction (bge is a pseudo-instruction implemented as blez after slt)
        //         Here we treat it as blez: branch if $s0 - $a1 <= 0 is false, i.e., branch if rs >= rt
        //         The parser encodes it similarly to blez/bgez using opcode 0b000001
        // Q2: What is the machine language data format?
        // Answer: oooooo ss sss ttttt iiiiiiii iiiiiiii
        // needs: opcode, rs, rt, immediate (branch offset in words)
        // Q3: What is the opcode of this instruction?
        // Answer: 0b000001 (0x01) - REGIMM opcode used for bgez
            opcode = 0b000001;
            machineCode = buildIInstruction(opcode,
                                            instructionStorage[i].rs,
                                            instructionStorage[i].rt,
                                            instructionStorage[i].immediate);
            write_dword(base_memory_address, codeOffset + i * 4, machineCode);
        }
        // ---- lw instruction ----
        else if (strcmp(instructionStorage[i].instruction, "lw") == 0){
        // Q1: What type of instruction is this? J, R or I instruction?
        // Answer: I-type instruction
        // Q2: What is the machine language data format?
        // Answer: oooooo ss sss ttttt iiiiiiii iiiiiiii
        // needs: opcode, rs (base register), rt (destination register), immediate (offset)
        // Q3: What is the opcode of this instruction?
        // Answer: 0b100011 (0x23) - load word opcode = 35
            opcode = 0b100011;
            machineCode = buildIInstruction(opcode,
                                            instructionStorage[i].rs,
                                            instructionStorage[i].rt,
                                            instructionStorage[i].immediate);
            write_dword(base_memory_address, codeOffset + i * 4, machineCode);
        }
        // ---- sw instruction ----
        else if (strcmp(instructionStorage[i].instruction, "sw") == 0){
        // Q1: What type of instruction is this? J, R or I instruction?
        // Answer: I-type instruction
        // Q2: What is the machine language data format?
        // Answer: oooooo ss sss ttttt iiiiiiii iiiiiiii
        // needs: opcode, rs (base register), rt (source register to store), immediate (offset)
        // Q3: What is the opcode of this instruction?
        // Answer: 0b101011 (0x2B) - store word opcode = 43
            opcode = 0b101011;
            machineCode = buildIInstruction(opcode,
                                            instructionStorage[i].rs,
                                            instructionStorage[i].rt,
                                            instructionStorage[i].immediate);
            write_dword(base_memory_address, codeOffset + i * 4, machineCode);
        }
        // ---- add instruction ----
        else if (strcmp(instructionStorage[i].instruction, "add") == 0){
        // Q1: What type of instruction is this? J, R or I instruction?
        // Answer: R-type instruction
        // Q2: What is the machine language data format?
        // Answer: oooooo sssss ttttt ddddd aaaaa ffffff
        // needs: opcode (0), rs, rt, rd, shamt (0), function code
        // Q3: What is the opcode/function of this instruction?
        // Answer: opcode = 0b000000 (0x00), function = 0b100000 (0x20) = 32
            opcode = 0b000000;
            machineCode = buildRInstruction(opcode,
                                            instructionStorage[i].rs,
                                            instructionStorage[i].rt,
                                            instructionStorage[i].rd,
                                            instructionStorage[i].shamt,
                                            0b100000);  // function code for add = 32
            write_dword(base_memory_address, codeOffset + i * 4, machineCode);
        }
        // ---- addi instruction ----
        else if (strcmp(instructionStorage[i].instruction, "addi") == 0){
        // Q1: What type of instruction is this? J, R or I instruction?
        // Answer: I-type instruction
        // Q2: What is the machine language data format?
        // Answer: oooooo ss sss ttttt iiiiiiii iiiiiiii
        // needs: opcode, rs (source), rt (destination), immediate (value to add)
        // Q3: What is the opcode of this instruction?
        // Answer: 0b001000 (0x08) - add immediate opcode = 8
            opcode = 0b001000;
            machineCode = buildIInstruction(opcode,
                                            instructionStorage[i].rs,
                                            instructionStorage[i].rt,
                                            instructionStorage[i].immediate);
            write_dword(base_memory_address, codeOffset + i * 4, machineCode);
        }
        // ---- j instruction ----
        else if (strcmp(instructionStorage[i].instruction, "j") == 0){
        // Q1: What type of instruction is this? J, R or I instruction?
        // Answer: J-type instruction
        // Q2: What is the machine language data format?
        // Answer: oooooo iiiiiiii iiiiiiii iiiiiiii iiiiiiii
        // needs: opcode and 26-bit jump target address
        // Q3: What is the opcode of this instruction?
        // Answer: 0b000010 (0x02) - jump opcode = 2
            opcode = 0b000010;
            machineCode = buildJInstruction(opcode,
                                            instructionStorage[i].immediate);
            write_dword(base_memory_address, codeOffset + i * 4, machineCode);
        }
        // ---- end of code ----
        else if (strcmp(instructionStorage[i].instruction, "syscall") == 0 ||
                 strcmp(instructionStorage[i].instruction, "END") == 0){
            // End of program reached - stop processing instructions
            break;
        }
        i++;
    } while (1);

    totalinstruction = i;  // total number of instructions processed
}

// ---------------------------------------------------------------
// loadCodeToMem: Top-level function called from main.c.
// Loads both the data section and code section into the 1MB memory space.
void loadCodeToMem(char *mem){
    unsigned int dataSection = DATASECTION;       // data starts at 0x2000
    unsigned int instructionSection = CODESECTION; // code starts at 0x0000

    // Step 1: Copy the .data section bytes into memory starting at 0x2000
    setupDataMemory(mem, dataSection, Data_storage, totalDataByte);

    // Step 2: Convert all instructions in Instruction_storage[] to 32-bit
    // machine code and write them into memory starting at 0x0000
    setupInstructionMemory(mem, instructionSection, Instruction_storage);

    // Step 3: Display memory dump of both sections for verification
    puts("\n---- Data Section ----\n");
    memory_dump(mem, DATASECTION, 256);   // show 256 bytes of data section
    puts("\n---- Code Section ----\n");
    memory_dump(mem, CODESECTION, 256);   // show 256 bytes of code section
}

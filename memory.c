/*
 ============================================================================
 Name        : memory.c
 Author      : Austin Tian
 Revised by  : Solomon Ewusi | Student ID: N01659373
 Version     :
 Copyright   : Copyright 2023
 Description : Memory controller for Lab 7 - initializes memory with zeros,
               supports byte and double-word read/write, and memory dump.
 ============================================================================
 */

#include "header.h"

// Menu string (kept for compatibility, not used in Lab 7+)
char *menu = "\n" \
             " ***********Please select the following options**********************\n" \
             " *    This is the memory operation menu                             *\n" \
             " ********************************************************************\n";

//---------------------------------------------------------------
// Generate a random number between 0x00 and 0xFE.
unsigned char rand_generator()
{
    return rand() % 255;
}

//---------------------------------------------------------------
// Free the allocated memory to avoid memory leakage.
void free_memory(char *base_address)
{
    free(base_address);
    return;
}

//---------------------------------------------------------------
// Allocate 1MB of memory and initialize all bytes to 0.
// Lab 7+ uses 0 instead of random numbers.
char *init_memory()
{
    char *mem = malloc(MEM_SIZE);  // allocate 1MB
    int i;
    for (i = 0; i < MEM_SIZE; i++){
        mem[i] = 0;  // initialize all bytes to zero
    }
    return mem;
}

//---------------------------------------------------------------
// Write a single byte to (base_address + offset).
void write_byte(const char *base_address, const int offset, const unsigned char byte_data)
{
    *(unsigned char *)(base_address + offset) = byte_data;
}

//---------------------------------------------------------------
// Write a 32-bit double-word to (base_address + offset).
void write_dword(const char *base_address, const int offset, const unsigned int dword_data)
{
    unsigned int *target = (unsigned int *)(base_address + offset);
    *target = dword_data;
}

//---------------------------------------------------------------
// Read a single byte from (base_address + offset).
unsigned char read_byte(const char *base_address, int offset)
{
    unsigned char data = *(unsigned char *)(base_address + offset);
    if (DEBUG_CODE)
        printf("Byte at offset 0x%X: 0x%02X\n", offset, data);
    return data;
}

//---------------------------------------------------------------
// Read a 32-bit double-word from (base_address + offset).
unsigned int read_dword(const char *base_address, int offset)
{
    unsigned int data = *(unsigned int *)(base_address + offset);
    if (DEBUG_CODE)
        printf("DWord at offset 0x%X: 0x%08X\n", offset, data);
    return data;
}

//---------------------------------------------------------------
// Display dumpsize bytes starting at (base_address + offset).
// Shows hex values and ASCII characters (non-printable = '.').
void memory_dump(const char *base_address, const int offset, unsigned int dumpsize)
{
    if (dumpsize < MIN_DUMP_SIZE || dumpsize > MEM_SIZE)
        dumpsize = MIN_DUMP_SIZE;

    unsigned int row, col;
    unsigned int num_rows = dumpsize / DUMP_LINE;

    for (row = 0; row < num_rows; row++)
    {
        const unsigned char *row_addr = (const unsigned char *)(base_address + offset + row * DUMP_LINE);
        printf("%p: ", (void *)row_addr);

        for (col = 0; col < DUMP_LINE; col++)
            printf("%02X ", row_addr[col]);

        printf("--- ");

        for (col = 0; col < DUMP_LINE; col++)
        {
            unsigned char c = row_addr[col];
            if (c >= 0x20 && c <= 0x7E)
                printf("%c", c);
            else
                printf(".");
        }
        printf("\n");
    }
    return;
}

//---------------------------------------------------------------
// setup_memory: interactive menu (used in Lab 6 only, kept for compatibility)
void setup_memory()
{
    char *mem = init_memory();
    free_memory(mem);
    return;
}

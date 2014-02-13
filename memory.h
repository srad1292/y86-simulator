#define MEMSIZE 1024     //1024 words of memory
#define MAXBYTE 4096     //4096 bytes in memory
#ifndef MEMORY_H
#define MEMORY_H

/**
 * The function prototypes that list
 * the functions in memory.c, their return types
 * and the paramaters for each.
 */

unsigned int fetch(int address, bool *memError);
void store(int address, unsigned int value, bool * memError);
unsigned char getByte(int address, bool * memError);
void putByte(int address, unsigned char value, bool * memError);
void clearMemory();
unsigned int getWord(int address, bool * memError);
void putWord(int address, unsigned int value, bool * memError);
#endif 


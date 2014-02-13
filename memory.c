#include <stdio.h>
#include "bool.h"
#include "memory.h"
#include "tools.h"

static unsigned int memoryArray[MEMSIZE];

/**
 * fetch
 * This function returns a word of memory from our memory array
 * @param: address: the address of memory to return
 *         memError: a bool that says if the address is valid
 * @return: memoryArray[address]: The word to be returned
 */
unsigned int fetch(int address, bool * memError){
    *memError = TRUE;
    if(address >= 0 && address < MEMSIZE) {
        *memError = FALSE;
        return memoryArray[address];
    }
    else {
        return 0;
    }
}

/**
 * store
 * This function places a word into the memoryArray
 * @param: address: the location in memory to store a word
 *         value: the word to store
 *         memError: bool that states if address is valid
 */

void store(int address, unsigned int value, bool * memError){
     *memError = TRUE;
     if(address >= 0 && address < MEMSIZE) {
         *memError = FALSE;
         memoryArray[address] = value;
     }
}

/**
 * getByte
 * This function returns a single byte of the main memory
 * @param: address: The address of the byte of memory to return
 *         memError: bool that says if the address is valid
 * @return: getByteNumber(..,..): The byte of memory
 */

unsigned char getByte(int address, bool * memError){
    
    *memError = TRUE;
    
    if(address >= 0 && address < MAXBYTE){
        *memError = FALSE;
        //return memoryArray[address];
        return getByteNumber(address % 4, memoryArray[address/4]);
    }
    else {
        return 0;        
    } 
}

/*
 * putByte
 * This function sets a specified byte to a vale
 * @param: address: The location in memory to set
 *         value: The value to set the byte to
 *         memError: Bool that says if address is valid
 */

void putByte(int address, unsigned char value, bool * memError){
     
     if(address < 0 || address >= MAXBYTE) {
        *memError = TRUE;
     }

     else {
        *memError = FALSE;           
        memoryArray[address/4] = putByteNumber(address%4
            , value, memoryArray[address/4]);
             
     }
   
}

/**
 * clearMemory
 * This function initializes the contents of memory to 0
 */
void clearMemory(){
    int run;
    for(run = 0; run < memoryArray[MEMSIZE]; run++){
        memoryArray[run] = 0;
    }
}

/**
 * getWord
 * This function returns a word from memory
 * @param: address: The address in memory of the word
 *         memError: bool that says if address is valid
 * @return: memoryArray[address/4]: The word in memory
 */

unsigned int getWord(int address, bool * memError){
    
    *memError = TRUE;
    if(address%4 == 0 && address >= 0 && address < MAXBYTE) {
        *memError = FALSE;
         return fetch(address/4, memError);         
    }
    else 
        return 0;
}

/**
 * putWord
 * This function places a word into memory location
 * @param: address: The location to place the word
 *         value: The value to store in memory
 *         memError: bool that says if address is valid
 */
void putWord(int address, unsigned int value, bool * memError){
    
    *memError = TRUE;
    if(address%4 == 0 && address >= 0 && address < MAXBYTE) {
        *memError = FALSE;
         store(address/4, value, memError);
    }
   
}


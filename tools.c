#include <stdio.h>
#include "bool.h"
#include "tools.h"

/**
 * Sam Radford
 * 8-28-13
 */

/* 
   Replace the return 0 with the code to perform the required function.
   Do not change the function prototypes.
   Be sure to check for errors in input. 
   Document EACH function with a function header (description, parameters,
   return).
*/

/**
 * int getBits
 * This function takes a range of bits from a number
 * and returns what those bits equal
 * @param: low: the low bit in the range, high: the high bit in the range,
 *         source, the unsigned int to be worked with
 * @return: source: The shifted unsigned int
 */

unsigned int getBits(int low, int high, unsigned int source)
{
    //The if statement checks that low and high are valid
    if(low < 0 || low > 31 || high < 0 || high > 31 || high < low)
       source=0;
    else{
        source = source << (31-high);        //Shifts the bits left
        source = source >> (31-(high-low));  //Shift the bits right
        }
    return source;
} 

/**
 * int setBits
 * This function takes a number and replaces the bits
 * in a range given with 1 and returns the value
 * that, that creates
 * @param: low: lowest bit in range, high: high bit in range,
 *         source: the unsigned int to be worked with
 * @return: source: the value with bits replaced
 */

unsigned int setBits(int low, int high, unsigned int source)
{
    //The if statement checks that low and high are valid
    if(low < 0 || low > 31 || high < 0 || high > 31 || high < low){
        return source;
    }   
    else{
        unsigned int largest = 0xffffffff;    //Creates a value with all bits being 1 
        largest = largest << (31-(high));     
        largest = largest >> (31-(high-low)); //Set bits in range to 1 
        largest = largest << low;
        source = (source | largest);          //ORs the bits of the source and largest
    
    }
    return source;
}

/**
 * clearBits
 * This function takes a number and replaces the bits
 * in a range given with 0 and returns the value
 * that, that creates
 * @param: low: low bit in the range, high: high bit in range,
 *         source: unsigned int to work with
 * @return: source: the value with bits replaced
 */

unsigned int clearBits(int low, int high, unsigned int source)
{
    if(low < 0 || low > 31 || high < 0 || high > 31 || high < low){
        return source;
    }
    else{
        unsigned int largest = 0xffffffff; //value with all bits set to 1
        largest = largest << (31-high);
        largest = largest >> (31-(high-low));
        largest = largest << low;
        largest = ~largest;                //flips bits so range = 0 
        source = (source & largest);       //ANDs the bits 
    }
    return source;
}


/**
 * assignOneBit
 * This function takes a bitValue and a bitNumber
 * and sets the bit at bitNumber to bitValue
 * @Param: bitNumber: The bit to set, bitValue: The value 
 *         source: The unsigned int to work with
 * @Return: source: The unsigned int with a changed bit
 */

unsigned int assignOneBit(int bitNumber, int bitValue, unsigned int source)
{
    if(bitNumber < 0 || bitNumber > 31 || bitValue < 0 || bitValue > 1)
        return source;
    else{
        if(bitValue == 0)
            //Sets the bit to 0
            source = clearBits(bitNumber,bitNumber, source);
        else
            //Sets the bit to 1
            source = setBits(bitNumber,bitNumber, source);
    }
    return source;
}

/**
 * getByteNumber
 * This function returns the value of a byte
 * @Param: byteNo: The byte to return
 *         source: The unsigned int to grab the byte
 * @Return: source: The value of the byte
 */

unsigned char getByteNumber(int byteNo, unsigned int source)
{
    if(byteNo < 0 || byteNo>3) 
        return 0;
    else {
        if(byteNo == 0){ 
            source = source << 24;         //Get byte 0
            source = source >> 24;
        }
        else if(byteNo == 1){
            source = source << (16);
            source = source >> (24);   //Get byte 1
        }
        else if(byteNo == 2){
            source = source << 8;        //Get byte 2
            source = source >> (24);
        }
        else if(byteNo == 3){
            source = source >> (24);   //Get byte 3    
        }
    }
    
    return source;
}

/**
 * putByteNumber
 * This function replaces a byte in a value with 
 * a given value
 * @Param: byteNo: The byte to change, 
 *         byteValue: What to set the byte as
 *         source: The value to edit
 * @Return: source: The value with the byte changed
 */

unsigned int putByteNumber(int byteNo, unsigned char byteValue, 
                           unsigned int source)
{
    if(byteNo < 0 || byteNo > 3) {
        return source;
    }
    else{
        unsigned int intByte = byteValue;
        if (byteNo == 0) {
            //This step replaces the given range with 0
            //So that you can use a bitwise or to set
            //The given byte with the new value
            source = clearBits(0,7,source); 
            
        }

        else if (byteNo == 1) {
            source = clearBits(8,15,source);
            intByte = intByte << 8;
        }

        else if (byteNo == 2) {
            source = clearBits(16,23,source);
            intByte = intByte <<16;
        }

        else {
            source = clearBits(24,31,source);
            intByte = intByte << 24;
        }

        //Set the specified byte to a new value
        source = source | intByte;    
    }
    return source;
}

/**
 * buildWord
 * This function takes 4 byte values and combines them
 * to make one unsigned int
 * @Param: byte0: The 0th byte, byte1: The 1st byte
 *         byte2: The 2nd byte, byte3: The 3rd byte
 * @Return: source: The unsigned int created
 */
unsigned int buildWord(unsigned char byte0, unsigned char byte1, 
                       unsigned char byte2, unsigned char byte3)
{
    unsigned int source = 0x0;
    //These 4 lines set the bytes 
    source = putByteNumber(0,byte0,source);
    source = putByteNumber(1,byte1,source);
    source = putByteNumber(2,byte2,source);
    source = putByteNumber(3,byte3,source);
    return source;
}

/**
 * isNegative
 * This function determines if a value is
 * positive or negative
 * @Param: Source: The value to check
 * @Return: isNeg: Returns 1 if it's negative
 */

bool isNegative(unsigned int source)
{
    /**
    int isNeg = 0;
    source = source >> 31;  //Clears all but leading bit
    if(source>=1)           //If leading bit = 1
      isNeg = 1;
    */
    return (source>>31);    
}

/**
 * expandBits
 * This function takes a value and an array
 * and fills the array so that it will
 * contain a character representation of the number
 * @Param: source: The value to display
 *         bits: The array to fill
 */

void expandBits(unsigned int source, char bits[36])
{
    int cond = 0;
    unsigned int bit; 
    unsigned int tempSource;
    //The loop goes through the first 35 array locations
    for(cond = 0; cond < 35; cond++) {
        //Fills a space after every 8 bits
        if(cond == 8 || cond == 17 || cond == 26) {
            bits[cond] = ' ';
        }
        else {
            tempSource = source;
            bit = tempSource >>31; //Gets the leading bit
            source = source << 1;  //Replaces the leading bit
            if(bit == 0) {
                bits[cond] = 48;   
            }
            else{
                bits[cond] = 49;
            }
        }
    }
    bits[35] = 0;
    return;
}

/**
 * clearBuffer
 * This function takes a pointer and the size
 * of the bytes of data the pointer points to
 * and makes each byte 0
 * @Param: buff: The pointer
 *         size: The size of data
 */

void clearBuffer(char * buff, int size)
{
    int cond = 0;
    for(cond = 0; cond < size; cond++) {
        (*buff) = 0;
        buff++;
    }
    return;
}


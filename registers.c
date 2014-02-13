#include <stdio.h>
#include "bool.h"
#include "registers.h"
#include "tools.h"

static unsigned int registers[REGSIZE]; //array of registers
static unsigned int CC;                 //Condition code

/**
 * getRegister
 * This function returns the contents of a register
 * @param: regNum: The register to return
 * @return: registers[regNum]: Contents of a register
 */

unsigned int getRegister(int regNum){
    if(regNum >= EAX && regNum <= EDI){
        return registers[regNum];
    }

    else {
        return 0;
    }
}

/**
 * setRegister
 * This function sets the contents of a register to a value
 * @param: regNum: The register number to set
 *         regValue: The value to store in the register
 */

void setRegister(int regNum, unsigned int regValue){
    if(regNum >= EAX && regNum <= EDI ){ 
      registers[regNum] = regValue;     
    }
}

/**
 * clearRegisters
 * This function initializes all registers to 0
 */

void clearRegisters(){
     int loop = 0;
     for(loop = 0; loop<REGSIZE; loop++){
         registers[loop] = 0;
     }
}

/**
 * setCC
 * This function sets a specific bit in the condition code
 * @param: bitNumber: the bit of the CC to set
 *         value: The value to set the bit to(1 or 0)
 */

void setCC(unsigned int bitNumber, unsigned int value){
    if(value == 0 || value == 1) {
       CC = assignOneBit(bitNumber, value, CC);        
    }
    //printf("SF(registers) = %x\n", getCC(SF));
}

/**
 * getCC
 * This function returns a bit of the condition code
 * @param: bitNumber: The bit to get
 * @return: getBits(..,..,..): The value of the bit
 */

unsigned int getCC(unsigned int bitNumber){
    
    return getBits(bitNumber,bitNumber,CC);
}


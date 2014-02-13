/**
 * Call writebackStage
 * This class handles the execution
 * of the writebackStage
 * of the pipeline 
 * Sam Radford and Alex Fitzgerald
 */

#include <stdio.h>
#include "bool.h"
#include "tools.h"
#include "registers.h"
#include "forwarding.h"
#include "status.h"
#include "bubbling.h"
#include "instructions.h"
#include "writebackStage.h"

bool handleDumping();
//W register holds the input for the write stage.
//It is only accessible from this file. (static)
static wregister W;

// Function: getWregister
// Description: Returns a copy of the W register
// Params: none
// Returns: wregister
// Modifies: none
wregister getWregister()
{
    return W;
}

// Function: clearWregister
// Description: Returns a copy of the W register
// Params: none
// Returns: none
// Modifies: W
void clearWregister()
{
    clearBuffer((char *) &W, sizeof(W));
    W.stat = SAOK;
    W.icode = NOP;
}


/**
 * Function: writebackStage
 * This drive function handles the bool
 * value that determines when the pipline
 * machine should stop executing. It also handles
 * the use of dumps.
 * Parameter: forward - A pointer to a forwardType instance
 * Return: retVal - Returns true when the input line
 *                  has an error or when the EoF
 *                  has been reached
 */
bool writebackStage(forwardType * forward, statusType * status, bubbleType bubble){
    if(W.icode == NOP){
        status->W_stat = SAOK;
        W.stat = SAOK;
        }
    if (handleDumping())
        return TRUE;
    bool *err = FALSE;
    //Forwarding from writebackStage
    (*forward).W_dstE = W.dstE;
    (*forward).W_valE = W.valE;
    (*forward).W_valM = W.valM;
    (*forward).W_dstM = W.dstM;
    (*forward).W_icode = W.icode;
    //Update statusType struct
    if(W.icode == NOP)
        status->W_stat = SAOK;
    else
        (*status).W_stat = W.stat;
    
   // handleDumping();
   // if(W.stat == SADR || W.stat == SINS || W.stat == SHLT) {
   //     retVal = TRUE;
   // }
          
    setRegister(W.dstE,W.valE);

    if(W.icode == IPUSHL||W.icode == IPOPL)
       setRegister(RESP,W.valE);

    setRegister(W.dstM,W.valM);

    return FALSE;
}

/**
 * Function: updateWStage
 * This function updates the values in writebackStage
 */
void updateWStage(unsigned int stat, unsigned int icode, unsigned int valE, unsigned int valM, unsigned int dstE, unsigned int dstM) {
    W.stat = stat;
    W.icode = icode;
    W.valE = valE;
    W.valM = valM;
    W.dstE = dstE;
    W.dstM = dstM;
}


/**
 * Function: handleDumping
 * This function handles the output for stats
 * and the dumping required for the dump instruction
 * and for SADR and SINS stats
 */
bool handleDumping(){
//If the stat in writebackStage is not SAOK
//Then a certain message should be printed
//and everything should be dumped
if(W.stat == SADR || W.stat == SINS || W.stat == SHLT) {
    if(W.stat != SHLT){
       if (W.stat == SADR)
           printf("Invalid memory address\n");

       if (W.stat == SINS)
           printf("Invalid instruction\n");

       dumpProgramRegisters();
       dumpProcessorRegisters();
       dumpMemory();
   }
   return TRUE;
}

//If the icode in writebackStage is DUMP
//Then based on which value is in valE
//certain dumps will be called
else{
    if(W.icode == DUMP) {
      if(getBits(0,0,W.valE) == 1) {
         dumpProgramRegisters();
      }
      if(getBits(1,1,W.valE)== 1) {
         dumpProcessorRegisters();
      }

      if(getBits(2,2,W.valE)==1) {
        dumpMemory();
      }
   }
   return FALSE;
}

}

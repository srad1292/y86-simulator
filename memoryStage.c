/**
 * Class: memoryStage
 * This class handles the
 * execution of the memoryStage
 * of the pipeline
 * Sam Radford and Alex Fitzgerald
 */


#include <stdio.h>
#include "bool.h"
#include "tools.h"
#include "forwarding.h"
#include "status.h"
#include "bubbling.h"
#include "instructions.h"
#include "memoryStage.h"
#include "writebackStage.h"


//M register holds the input for the memory stage.
//It is only accessible from this file. (static)
static mregister M;
unsigned int memCond(unsigned int valE,bool * memError);
bool memRead();
bool memWrite();
unsigned int memAddr();
unsigned int setStat(bool  memError);
bool W_stall(bubbleType bubble, statusType status);

/**
 * Function: memoryStage
 * This driver function updates the values in writebackStage
 */
void memoryStage(forwardType * forward, statusType * status, bubbleType * bubble) {
    unsigned int valM;
    unsigned int valE;
    bool memError = FALSE;
    valE = memAddr();
    valM = memCond(valE,&memError);
    if(!memRead())
        valM = 0;
    //Forwarding from memoryStage
    (*forward).m_valM = valM;
    (*forward).M_valE = M.valE;

    (*forward).M_dstE = M.dstE;
    (*forward).M_dstM = M.dstM;
    (*forward).M_Cnd = M.Cnd; 
    (*forward).M_icode = M.icode;
    (*forward).M_valA = M.valA;
    unsigned int mStat;
    mStat = setStat(memError);
    //Updating stat in the statusType struct
    //and the bubbleType struct
    (*status).m_stat = mStat;
    (*bubble).M_icode = M.icode;
    if(!(W_stall(*bubble,*status)))
        updateWStage(mStat,M.icode,M.valE,valM,M.dstE,M.dstM);
}

/**
 * Function: memAddr
 * This function returns the value to be used when
 * reading or writing from memory in this stage
 * Return: valE or valA
 */
unsigned int memAddr() {
    if(M.icode ==IRMMOVL || M.icode == IMRMOVL || M.icode == IPUSHL||M.icode == ICALL){
        return M.valE;
    }
    else if(M.icode == IPOPL || M.icode == IRET){
        return M.valA;
    }
    else
        return M.valE;
}

/**
 * Function: memCond
 * This function returns the value to be sent as valM
 * and reads or writes from memory if needed
 * Return: value to be used as valM
 */
unsigned int memCond(unsigned int valE,bool * memError) {
     bool tempEr = FALSE;
     unsigned int ret = M.valA;
     if(memRead()){
         ret = getWord(valE,&tempEr);
         *memError = tempEr; 
     }
     else if(memWrite()){
         putWord(valE,M.valA,&memError);
         ret = M.valA;
     }
     return ret;
}

/**
 * Function memRead
 * This function determines whether or not
 * the current instruction needs to read from
 * memory
 * Return: True or False
 */
bool memRead(){
    if(M.icode == IMRMOVL || M.icode == IPOPL || M.icode == IRET)
        return TRUE;
    else
        return FALSE;
}

/**
 * Function memWrite
 * This function determines whether or not
 * the current instructions needs to write
 * to memory
 * Return: True or False
 */
bool memWrite(){
    if(M.icode == IRMMOVL || M.icode == IPUSHL || M.icode == ICALL)
        return TRUE;
    else
        return FALSE;
}


// Function: getMregister
// Description: Returns a copy of the M register
// Params: none
// Returns: mregister
// Modifies: none
mregister getMregister()
{
    return M;
}

// Function: clearMregister
// Description: Returns a copy of the M register
// Params: none
// Returns: none
// Modifies: M
void clearMregister()
{
    clearBuffer((char *) &M, sizeof(M));
    M.stat = SAOK;
    M.icode = NOP;
}

/**
 * Function: updateMregister
 * This function updates the values in
 * memory stage
 */
void updateMregister(unsigned int stat, unsigned int icode,
     unsigned int Cnd, unsigned int valE, unsigned int valA, 
     unsigned int dstE, unsigned int dstM){
     M.stat = stat;
     M.icode = icode;
     M.Cnd = Cnd;
     M.valE = valE;
     M.valA = valA;
     M.dstE = dstE;
     M.dstM = dstM;      
}

/**
 * Function setStat
 * This function returns what stat to send to
 * writebackStage and to the statusType struct
 * Parameter: memError: Bool saying if there is a memory error
 * Return: SADR if a memory error occured, otherwise return M.stat
 */
unsigned int setStat(bool  memError){
    if(memError == 1)
        return SADR;
    else
        return M.stat;
      
}

/**
 * Function W_stall:
 * This function determines if 
 * the writebackStage should be bubbled
 * @Param: bubbleType instance, statusType instance
 * @Return: If the writebackStage should be bubbled
 */
bool W_stall(bubbleType bubble, statusType status)
{
    if(status.W_stat != SAOK) 
        return TRUE; 
    else 
        return FALSE;

}

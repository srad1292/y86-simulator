/**
 * Class: decodeStage
 * This class handles the
 * execution of the decodeStage
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
#include "registers.h"
#include "decodeStage.h"
#include "executeStage.h"


//D register holds the input for the decode stage.
//It is only accessible from this file. (static)
static dregister D;
unsigned int getSrcB(unsigned int);
unsigned int getSrcA(unsigned int);
unsigned int getDstE(unsigned int);
unsigned int getDstM(unsigned int);
unsigned int selectFwdA(forwardType, unsigned int);
unsigned int forwardB(forwardType, unsigned int);

// Function: getDregister
// Description: Returns a copy of the D register
// Params: none
// Returns: dregister
// Modifies: none
dregister getDregister()
{
        return D;
}

//
//    // Function: clearDregister
//    // Description: Returns a copy of the D register
//    // Params: none
//    // Returns: none
//    // Modifies: D
void clearDregister()
{
    clearBuffer((char *) &D, sizeof(D));
    D.stat = SAOK;
    D.icode = NOP;
}

/**
 * Function: decodeStage
 * The driver function for decode stage
 * of the pipeline machine.  The function
 * updates the values in execute stage
 */
void decodeStage(forwardType forward, bubbleType * b)
{ 
    unsigned int dstE = getDstE(D.icode);
    unsigned int dstM = getDstM(D.icode);
    unsigned int srcA = getSrcA(D.icode);
    unsigned int srcB = getSrcB(D.icode);
    unsigned int valA = selectFwdA(forward, srcA);
    unsigned int valB = forwardB(forward, srcB);
    (*b).d_srcA = srcA;
    (*b).d_srcB = srcB;
    (*b).D_icode = D.icode;

    if(bubbleEregister(*b))
        updateEregister(SAOK, NOP, 0, 0, 0, 0, RNONE, RNONE, RNONE, RNONE);
    else 
        updateEregister(D.stat, D.icode, D.ifun, D.valC, valA, valB, dstE, dstM, srcA, srcB);    
}

/**
 * Function: updateDregister
 * This fucntion updates the values of decode stage
 */
void updateDregister(unsigned int stat, unsigned int icode, unsigned int ifun, 
                        unsigned int rA, unsigned int rB, unsigned int valC, unsigned int valP)
{
    D.stat = stat;
    D.icode = icode;
    D.ifun = ifun;
    D.rA = rA;
    D.rB = rB;
    D.valC = valC;
    D.valP = valP;
}

/**
 * Function: getSrcA
 * This function returns
 * the value to be used for srcA
 * Return: D.rA for IOPL
 *         RNONE for anything else
 */
unsigned int getSrcA(unsigned int icode)
{
    if (icode == IRRMOVL || icode == IRMMOVL || icode == IOPL || icode == IPUSHL) { 
        return D.rA;
    } else if (icode == IPOPL || icode == IRET) {
        return RESP;
    } else return RNONE;
        
}

/**
 * Function: getSrcB
 * This function returns
 * the value to be used for srcB
 * Return: D.rB for IOPL
 *         RNONE for anything else
 */
unsigned int getSrcB(unsigned int icode)
{
    if (icode == IOPL || icode == IRMMOVL || icode == IMRMOVL) {
        return D.rB;
    } else if (icode == IPUSHL || icode == IPOPL || icode == ICALL || icode == IRET) {
        return RESP;
    } else return RNONE;
}

/**
 * Function getDstE
 * This function returns the value
 * To be used for the destination
 * of a register
 * Return: Destination register
 */
unsigned int getDstE(unsigned int icode)
{
    if(icode == IIRMOVL || icode == IRRMOVL || icode == IOPL){
        return D.rB;
    }else if(icode == IPUSHL || icode == IPOPL || icode == ICALL || icode == IRET){
        return RESP;
    }else return RNONE;
}

/*
 * Function getDstM
 * This function returns the
 * destination location in memory
 * Return: Memory Destination
 */
unsigned int getDstM(unsigned int icode)
{
    if(icode == IMRMOVL || icode == IPOPL){
        return D.rA;
    }else return RNONE;
}

/**
 * Function selectFwdA
 * This function returns the 
 * value to be sent to executeStage as
 * valA 
 * Paramter: d_srcA: Source register for A
 *           d_rvalA: Value to return dstE is different from srcA
 *           W_dstE: DestinationE from W
 *           W_valE: Value in destination
 * Return: The value to be used as valA in executeStage
 */
unsigned int selectFwdA(forwardType forward, unsigned int srcA) 
{
    if(D.icode == ICALL || D.icode == IJXX){ 
        return D.valP;
    }
    if (srcA == RNONE){ 
        return 0;
    }
    if (srcA == forward.e_dstE){ 
        return forward.e_valE;
    }
    if (srcA == forward.M_dstM){ 
        return forward.m_valM;  
    }
    if (srcA == forward.M_dstE){ 
        return forward.M_valE;
    }
    if (srcA == forward.W_dstM){ 
        return forward.W_valM;
    }
    if (srcA == forward.W_dstE){
        return forward.W_valE;
    }
    return getRegister(srcA);
}

/**
 * Function forwardB
 * This function returns the
 * value to be sent to executeStage as
 * valB
 * Paramter: d_srcB: Source register for B
 *           d_rvalB: Value to return dstE is different from srcA
 *           W_dstE: DestinationE from W
 *           W_valE: Value in destination
 * Return: The value to be used as valB in executeStage
 */
unsigned int forwardB(forwardType forward, unsigned int srcB)
{
    if(srcB == RNONE) return 0;

    if (srcB == forward.e_dstE) {
        return forward.e_valE;
    }

    if (srcB == forward.M_dstM) {
        return forward.m_valM;
    }

    if (srcB == forward.M_dstE) {
        return forward.M_valE;
    }

    if (srcB == forward.W_dstM) {
        return forward.W_valM;
    }

    if(srcB == forward.W_dstE){
        return forward.W_valE;
    } 
    
    return getRegister(srcB);
}

/**
 * Function: bubbleEregister
 * This function determines if
 * the e register should be bubbled
 * @Param: The instance of bubbleType
 * @Return: TRUE or FALSE
 */
bool bubbleEregister(bubbleType b)
{
    return ((b.E_icode == IJXX && !b.e_Cnd) ||
            ((b.E_icode == IMRMOVL || b.E_icode == IPOPL) &&
            (b.E_dstM == b.d_srcA || b.E_dstM == b.d_srcB)));
}    

/**
 * Class: executeStage
 * This class handles the
 * execution of the executeStage
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
#include "executeStage.h"
#include "memoryStage.h"


//E register holds the input for the execute stage.
//It is only accessible from this file. (static)
static eregister E;
static unsigned int(*funcPtrArr[FUN_ARRY])() = {NULL};
//Function stubs for
//Functions that will only be
//Used and called in executeStage
unsigned int setDstE(unsigned int icode, unsigned int *Cnd);
static unsigned int set_cc;
bool decBub(statusType status);
bool M_bubble(bubbleType bubble, statusType status);
void setFlags(int temp,int OFflag);
void setCnd(unsigned int * Cnd);
unsigned int halt(unsigned int * Cnd);
unsigned int dump(unsigned int * Cnd);
unsigned int nop(unsigned int * Cnd);
unsigned int cmovxx(unsigned int * Cnd);
unsigned int performIrmovl(unsigned int * Cnd);
unsigned int performRmmovl(unsigned int * Cnd);
unsigned int performMrmovl(unsigned int * Cnd);
unsigned int performOpl(unsigned int * Cnd);
unsigned int push(unsigned int * Cnd);
unsigned int pop(unsigned int * Cnd);
unsigned int jxx(unsigned int *Cnd);
unsigned int call(unsigned int *Cnd);
unsigned int ret(unsigned int *Cnd);

/**
 * Function: halt
 * Returns value to be used as valE
 * Return: 0
 */
unsigned int halt(unsigned int * Cnd){
    *Cnd = 0;
    return 0;
}

/**
 * Function: nop
 * Returns value to be used as valE
 * Return: 0
 */
unsigned int nop(unsigned int * Cnd) {
    *Cnd = 0;
    return 0;
}

/**
 * Function dump
 * Returns value to be used as valE
 * Return: valC from executeStage
 */
unsigned int dump(unsigned int * Cnd) {
    *Cnd = 0;
    return E.valC;
}


/**
 * Function: executeStage
 * This driver function gets the value to be used
 * for valE and then updates the values in memoryStage
 */
void executeStage(forwardType * forward, statusType status, bubbleType * bubble) {
    unsigned int Cnd;
    unsigned int valE;
    unsigned int tdstE = E.dstE;
    unsigned int tdstM = E.dstM;
    
    set_cc = shouldSetCC(status);  //Bool to set CC or not

    if(E.icode == IRRMOVL || E.icode == IJXX)
        setCnd(&Cnd);
    
    
    valE  = funcPtrArr[E.icode](&Cnd); 
    tdstE = setDstE(E.icode,&Cnd);
    //Handles the forwarding
    //and updating of the bubbling
    (*forward).e_dstE = tdstE;
    (*forward).e_valE = valE;
    (*forward).e_Cnd = Cnd;
    (*forward).e_valA = E.valA;
    (*bubble).E_dstM = E.dstM;
    (*bubble).e_Cnd = Cnd;
    (*bubble).E_icode = E.icode;
    //Bubbles if needed
    //Otherwise update normally
    if(decBub(status))
        updateMregister(SAOK,NOP,0,0,0,RNONE,RNONE);
    else
        updateMregister(E.stat,E.icode,Cnd,valE,E.valA,tdstE,E.dstM);
}

/**
 * Function: initializeFuncPtrArray
 * This functions sets what function
 * the array points to at each index
 */
void initializeFuncPtrArray() {
   int i;
   for(i = 0;i<FUN_ARRY;i++){
       funcPtrArr[i] = &nop;
   }
   //Initialze all of the function 
   //pointers in the array
   funcPtrArr[HALT] = &halt;
   funcPtrArr[NOP] = &nop;  
   funcPtrArr[DUMP] = &dump;
   funcPtrArr[CMOVXX] = &cmovxx;
   funcPtrArr[IIRMOVL] = &performIrmovl;
   funcPtrArr[IRMMOVL] = &performRmmovl;
   funcPtrArr[IMRMOVL] = &performMrmovl;
   funcPtrArr[IPUSHL] = &push;
   funcPtrArr[IPOPL] = &pop;
   funcPtrArr[IOPL] = &performOpl;
   funcPtrArr[IJXX] = &jxx;
   funcPtrArr[IRET] = &ret;
   funcPtrArr[ICALL] = &call;

}


// Function: getEregister
// Description: Returns a copy of the E register
// Params: none
// Returns: eregister
// Modifies: none
eregister getEregister()
{
     return E;
}

// Function: clearEregister
// Description: Returns a copy of the E register
// Params: none
// Returns: none
// Modifies: E
void clearEregister()
{
     clearBuffer((char *) &E, sizeof(E));
     E.stat = SAOK;
     E.icode = NOP;
}

/**
 * Function updateEregister
 * This function updates the values in execute stage
 */
void updateEregister(unsigned int stat, unsigned int icode, 
     unsigned int ifun, unsigned int valC, unsigned int valA,
     unsigned int valB, unsigned int dstE, unsigned int dstM, 
     unsigned int srcA, unsigned int srcB){
     E.stat = stat;
     E.icode = icode;
     E.ifun = ifun;
     E.valC = valC;
     E.valA = valA;
     E.valB = valB;
     E.dstE = dstE;
     E.dstM = dstM;
     E.srcA = srcA;
     E.srcB = srcB;
}

/**
 * Function shouldSetCC
 * This function decided based of
 * the status and the instruction icode
 * if the CC should be allowed to update
 * Parameter: stats: Instance of statustype 
 * Return: True or False
 */
bool shouldSetCC(statusType status){
    unsigned int a = status.m_stat;
    unsigned int b = status.W_stat;
  
    if(E.icode == IOPL){
        if(a != SADR && a != SINS && a != SHLT){
            if(b != SADR && b != SINS && b != SHLT){
              return TRUE;
            }
        }
    }

    return FALSE;
  

}

/**
 * Function decBub
 * This function determines, based off the
 * stat of memoryStage and writebackStage,
 * whether or not to bubble the M register
 * Parameter: status: Instance of statusType
 * Return: True or False
 */
bool decBub(statusType status){
    unsigned int tm = status.m_stat;
    unsigned int tw = status.W_stat;
    if(tm == SINS || tm == SADR || tm == SHLT)
        return TRUE;
    if(tw == SINS || tw == SADR || tw == SHLT)
        return TRUE;

    return FALSE;

}

/**
 * Function M_bubble
 * This function determines if 
 * the memory stage should be bubbled
 * Parameter: status: Instance of bubbleType, Instance of statusType
 * Return: True or False
 */

bool M_bubble(bubbleType bubble, statusType status)
{
    switch(status.m_stat){
        case SADR:
        case SINS:
        case SHLT:
            return TRUE;
    }

    switch(status.W_stat){
        case SADR:
        case SINS:
        case SHLT:
            return TRUE;
    }
    
    return FALSE;
    
}

/**
 * Function setFlags
 * This function sets the value for
 * the Sign Flag, Zero Flag, and Overflow Flag
 * is set_cc is TRUE
 * Parameter: temp - The result to base the flags on
 *            OFflag - The value to place in OF
 */ 
void setFlags(int temp,int OFflag){
    if(set_cc){
       if(temp < 0)
          setCC(SF,1);
       else
          setCC(SF,0);

       if(temp == 0)
          setCC(ZF,1);
       else
          setCC(ZF,0);     
        
    setCC(OF,OFflag);
    }
}


/**
 * Function setDstE
 * This function sets the value to be passed as dstE
 * Return: The value for dstE
 */
unsigned int setDstE(unsigned int icode, unsigned int *Cnd){
    if((icode == IRRMOVL) && (*Cnd==0))
        return RNONE;
    else
        return E.dstE;
}



/**
 * Function: performOpl
 * This function sets the condition codes
 * based on some arithematic operation and
 * returns the result of said arithematic result
 * Return: The result of one of - add,sub,and,xor
 */
unsigned int performOpl(unsigned int * Cnd){
    *Cnd = 0;
    int tempOP;
    switch(E.ifun){
     
    case ADDL:
        
        tempOP = (signed)E.valA + (signed)E.valB;        
        
        //Overflow if the sign of the two operands match
        //but the result is a different sign
        if(isNegative(E.valA) == isNegative(E.valB) 
            && isNegative(E.valA) != isNegative(tempOP))
            setFlags(tempOP,1);
        else
            setFlags(tempOP,0);
             
        return tempOP;
        break;

    case SUBL:
        tempOP = (signed)E.valB - (signed)E.valA;
        //Overflow if the sign of the two operands are different
        //and the result is the same sign as the operand being subtracted
        //from another operand
        if(isNegative(E.valA) != isNegative(E.valB)
          && isNegative(E.valA) == isNegative(tempOP))
          setFlags(tempOP,1);
        else
          setFlags(tempOP,0);
     
        return tempOP;
        break;
    
    case ANDL:
        tempOP = E.valA & E.valB;
        setFlags(tempOP,0);
        return tempOP;
        break;
    
    case XORL:
        tempOP = E.valA ^ E.valB;
        setFlags(tempOP,0);
        return tempOP;
        break;
    }
}

/**
 * Function: performIrmovl
 * This function returns the value
 * to be used for valE
 * Return: E.valC
 */
unsigned int performIrmovl(unsigned int * Cnd){
    *Cnd = 0;
    return E.valC;
}

/**
 * Function: cmovxx
 * This function returns
 * the value to be used for valE
 *
 */
unsigned int cmovxx(unsigned int * Cnd) {
    if(*Cnd == 1)
        return E.valA;
    else
        return RNONE;
}

/**
 * Function: setCnd
 * This function caluclates what to set the Cnd to(1 or 0)
 * based on the SF,OF,and ZF and the type of cmovxx
 * Parameter: *Cnd = A pointer to the Cnd field to send to memoryStage
 */
void setCnd(unsigned int *Cnd){
   
    switch(E.ifun){
    *Cnd = 0;
    case RRMOVL:
        *Cnd = 1;
        break;
    
    //Val <= 0
    //Also JLE
    case CMOVLE:
        *Cnd = ((getCC(SF)^getCC(OF))|getCC(ZF));
        break;
    //Val < 0
    //Also JL
    case CMOVL:
        *Cnd =(getCC(SF)^getCC(OF));
        break;
    //val = 0
    //Also JE
    case CMOVE:
        *Cnd =(getCC(ZF));
           
        break; 
    //Val != 0
    //Also JNE    
    case CMOVNE:
        *Cnd = (!getCC(ZF));
 
        break;
    //Val >= 0
    //Also JGE
    case CMOVGE:
        *Cnd = (!(getCC(SF)^getCC(OF)));
           
        break;
    //Val > 0
    //Also JG
    case CMOVG:
        *Cnd =(!(getCC(SF)^getCC(OF)) & !getCC(ZF));
        break;
    }
}

/**
 * Function performRmmovl
 * This function returns the value to be used as valE
 * Return: ValB+ValC
 */
unsigned int performRmmovl(unsigned int * Cnd){
    *Cnd = 0;
    return (E.valB + E.valC);
}

/**
 * Function performMrmovl
 * This function returns the value to be used as valE
 * Return: ValB+ValC
 */
unsigned int performMrmovl(unsigned int * Cnd){
    *Cnd = 0;
    return (E.valB + E.valC);
}

/**
 * Function push
 * This function returns the value to be used as valE
 * Return: valB-RESP(4)
 */
unsigned int push(unsigned * Cnd){
   *Cnd = 0;
   return (E.valB - RESP);
}

/**
 * Function pop
 * This function returns the value to be used as valE
 * Return: ValB+RESP(4)
 */
unsigned int pop(unsigned int * Cnd){
    *Cnd = 0;
    return (E.valB + RESP);
}

/**
 * Function jxx
 * This function returns the value
 * to be used as valE if the instruction
 * is a jump
 * @Return: The value to be used as valE
 */
unsigned int jxx(unsigned int * Cnd){
    if(*Cnd == 1)
        return E.valA;

    return RNONE;
}

/**
 * Function call
 * This function returns the value
 * to be used as valE if the instruction
 * is call
 * @Return: The value to be used as valE
 */
unsigned int call(unsigned int *Cnd){
    *Cnd = 0;
    return (E.valB - RESP);
}

/**
 * Function ret
 * This function returns the value
 * to be used as valE if the instruction
 * is a ret
 * @Return: The value to be used as valE
 */
unsigned int ret(unsigned int *Cnd){
    *Cnd = 0;
    return (E.valB + RESP);
}


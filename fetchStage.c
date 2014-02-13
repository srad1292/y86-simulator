/**
 * Class: fetchStage
 * This class handles the
 * execution of the fetchStage
 * of the pipeline
 * Sam Radford and Alex Fitzgerald
 */

#include <stdio.h>
#include "bool.h"
#include "tools.h"
#include "forwarding.h"
#include "bubbling.h"
#include "instructions.h"
#include "fetchStage.h"
#include "decodeStage.h"

//F register holds the input for the fetch stage. 
//It is only accessible from this file. (static)
static fregister F;
//Function stubs for
//functions that will only be 
//used and called in fetchStage
bool needs_regids(unsigned int icode);
unsigned int buildValC(unsigned int valP, bool memError);
bool needs_valC(unsigned int icode);
unsigned int predictPC(unsigned int valC, unsigned int valP,unsigned int icode);
unsigned int selectPC(unsigned int predPC,forwardType forward);
void updateFregister(unsigned int predPC);
unsigned int selectStat(unsigned int icode,bool mErr);
bool F_stall(bubbleType bubble);
bool D_stall(bubbleType bubble);
bool D_bubble(bubbleType bubble);

// Function: getFregister
// Description: Returns a copy of the F register
// Params: none
// Returns: fregister
// Modifies: none
fregister getFregister()
{
    return F;
}

// Function: clearFregister
// Description: Returns a copy of the F register
// Params: none
// Returns: none
// Modifies: F 
void clearFregister()
{
    clearBuffer((char *) &F, sizeof(F));
     
}

/**
 * Function: fetchStage
 * The driver function for the fetch stage 
 * of the pipeline machine.
 * The fetch statch fetches an instrction
 * and sets the icode and ifun
 * and updates the values in decodeStage
 */

void fetchStage(forwardType forward, bubbleType bubble){
    bool memError = FALSE;
    unsigned int f_pc = selectPC(F.predPC,forward);  //Sets the program counter
    unsigned char opcode;  // The instrction icode and ifun
    unsigned char rA = RNONE;
    unsigned char rB = RNONE;
    unsigned int icode,ifun,stat,valP,rI;
    unsigned int valC = 0;
    valP = f_pc;  //A temp value to update and eventually give to f_pc
    opcode = getByte(valP,&memError);
    valP++;
    
    //If there is no memory error
    //Set the icode and ifun
    if(!memError) {
           icode = getBits(4,7,opcode);
           ifun = getBits(0,3,opcode); 
    }

    //If there is a memory error
    //Set the icode to NOP
    else {
        icode = NOP;
        ifun = FNONE;
    }
   
    
    //If the function needs a register identifier 
    if(needs_regids(icode)) {       
       rI = getByte(valP,&memError);
       rA = getBits(4,7,rI);
       rB = getBits(0,3,rI);
       if(icode == IIRMOVL){
           rA = RNONE;
       }
        
       //Increment PC by 1 for ops or rr moves 
       else if(icode == IOPL || icode == IRRMOVL)
           valP+=1;
        
       //If PUSH or POP use RNONE as the rB register identifier
       //Increment the pc by 1 
       else if(icode == IPUSHL || icode == IPOPL){
           rB = RNONE;
           valP++;
       }
    }
    
    //Checks to see if the instruction
    //needs to build a valC
    //and if so it doest
    if(needValC(icode)){
        if(icode == DUMP || icode == IJXX || icode == ICALL){
            valC = buildValC(valP,memError);
            valP+=4;
        }
        else{
            valC = buildValC(valP+1,memError);
            valP+=5;
        }
    }
    
    //Gets the stat based on the instruction
    stat = selectStat(icode,memError);

    if(stat == SHLT)
        valC = 0;
    if(!(F_stall(bubble))) 
        //F.predPC = f_pc;
        F.predPC = predictPC(valC,valP,icode);
    
    //Bubble the d if needed
    //otherwise update normally
    if(D_bubble(bubble))
            updateDregister(SAOK,NOP,0,RNONE,RNONE,0,0);
    else if(!(D_bubble(bubble)) && !(D_stall(bubble)))
            updateDregister(stat,icode,ifun, rA, rB, valC,valP);
    
}


/**
 * Function: intr_valid
 * This function returns true if the instruction
 * matching the given instruction code is a valid
 * y-86 instruction
 *
 * Parameter: icode - The instruction code
 * Return: bool - True if a valid instruction
 */
bool intr_valid(unsigned int icode){
    //The valid instructions
    if(icode == HALT || icode == DUMP || icode == NOP)
        return TRUE;
    else if(icode == IIRMOVL || icode == IOPL)
        return TRUE;
    else if(icode == IRRMOVL || icode == IRMMOVL || icode == IMRMOVL)
        return TRUE;
    else if(icode == IPUSHL || icode == IPOPL)
        return TRUE;
    else if(icode == IJXX || icode == ICALL || icode == IRET)
        return TRUE;
    else
        return FALSE;
}

/**
 * Function selectPC
 * This functions chooses which value
 * will be selected as the program counter
 * Parameter: predPC - current F.predPC
 *            forward - The instance of the forwardType to retrieve
 *                      values from further down the pipe
 * Return: The new value to use as predPC
 */
unsigned int selectPC(unsigned int predPC, forwardType forward){
    if(forward.M_icode == IJXX && forward.M_Cnd == FALSE)
        return forward.M_valA;
    else if(forward.W_icode == IRET)
        return forward.W_valM;
    else 
        return F.predPC;
}

void updateFregister(unsigned int predPC) {
    F.predPC = predPC;
}


/**
 * Function: selectStat
 * This function returns the status code
 * for a given line of input based on if the 
 * instruction is valid, if the instruction is a halt, or
 * if the instruction has a memory error
 *
 * Paramter: icode - The instruction code
 * Return: The value of the status code(from instructions.h)
 */
unsigned int selectStat(unsigned int icode, bool mErr){
    
    if(mErr == TRUE)
        return SADR;
    if(!(intr_valid(icode)))
        return SINS;
    if(icode == HALT)
        return SHLT;

    return SAOK;
}

/**
 * Function: needs_regids
 * This function returns true if the given instruction 
 * needs a register identifier
 * Parameter: icode - The instruction code
 * Return: bool - True if instruction needs register identifier
 */
bool needs_regids(unsigned int icode){
    if(icode == IIRMOVL || icode == IOPL)
        return TRUE;
    else if(icode == IRRMOVL|| icode == IRMMOVL || icode == IMRMOVL)
        return TRUE;
    else if(icode == IPOPL || icode == IPUSHL)
        return TRUE;
    else
        return FALSE;
}

/**
 * Function: buildValC
 * This function Builds
 * the value that will
 * be used as valC if needed
 * Parameter: valP - current address
 *            memError - error in memory
 * Return: tvalC - the calculated value of valC
 */
unsigned int buildValC(unsigned int valP,bool memError){
    unsigned int temp1,temp2,temp3,temp4;
    unsigned int tvalC = 0;
    temp1 = getByte(valP,&memError);
    temp2 = getByte(valP+1,&memError);
    temp3 = getByte(valP+2,&memError);
    temp4 = getByte(valP+3,&memError);
    tvalC = buildWord(temp1,temp2,temp3,temp4);   
    return tvalC;
}

/**
 * Function predictPC
 * This function predicts the value to be used as the program counter
 * Parameter: valC: valC determined in fetch
 *            valP: Next sequential instruction
 *            icode: Icode of current instruction
 * Return: Value to use as the next PC
 */
unsigned int predictPC(unsigned int valC, unsigned int valP, unsigned int icode){
    if(icode == IJXX || icode == ICALL)
        return valC;
    
    return valP;
}
/**
 * Function needValc
 * This function determines if the icode needs valc or not
 * Parameter: icode: Icode of current instruction
 * Return: true or false if the icode needs valc
 */
bool needValC(unsigned int icode){
    switch(icode){
        case DUMP:
            return TRUE;
            break;
        case IIRMOVL:
            return TRUE;
            break;
        case IRMMOVL:
            return TRUE;
            break;
        case IMRMOVL:
            return TRUE;
            break;
        case IJXX:
            return TRUE;
            break;
        case ICALL:
            return TRUE;
            break;
        default:
            return FALSE;
    }
}

/**
 *Function F_stall
 *This function determines if the
 *fetch stage should be stalled
 *@Param: bubbleType variable
 *@Return: TRUE or FALSE
 */
bool F_stall(bubbleType bubble)
{
    bubbleType b = bubble;
    return (((b.E_icode == IMRMOVL || b.E_icode == IPOPL) &&
            (b.E_dstM == b.d_srcA || b.E_dstM == b.d_srcB)) ||
            (b.D_icode == IRET || b.E_icode == IRET || b.M_icode == IRET));    
}

/**
 *Function D_stall
 *This function determines if the
 *decode stage should be stalled
 *@Param: bubbleType variable
 *@Return: TRUE or FALSE
 */
bool D_stall(bubbleType bubble)
{
    bubbleType b = bubble;
    return ((b.E_icode == IMRMOVL || b.E_icode == IPOPL) &&
            (b.E_dstM == b.d_srcA || b.E_dstM == b.d_srcB));
}

/**
 *Function D_bubble
 *This function determines if the
 *decode stage should be bubbled
 *@Param: bubbleType variable
 *@Return: TRUE or FALSE
 */
bool D_bubble(bubbleType bubble)
{
    bubbleType b = bubble;
    return ((b.E_icode == IJXX && !b.e_Cnd) ||
            !((b.E_icode == IMRMOVL || b.E_icode == IPOPL) &&
            (b.E_dstM == b.d_srcA || b.E_dstM == b.d_srcB)) &&
            (b.D_icode == IRET || b.E_icode == IRET || b.M_icode == IRET));        
}

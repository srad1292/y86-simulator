#define FUN_ARRY 16 
#ifndef EXECUTESTAGE_H
#define EXECUTESTAGE_H
typedef struct
{
        unsigned int stat;
        unsigned int icode;
        unsigned int ifun;
        unsigned int valC;
        unsigned int valA;
        unsigned int valB;
        unsigned int dstE;
        unsigned int dstM;
        unsigned int srcA;
        unsigned int srcB;
} eregister;

//prototypes for functions called from files other than executeStage
void executeStage(forwardType * forward, statusType status, bubbleType * bubble);
eregister getEregister();
void clearEregister();
void updateEregister(unsigned int stat, unsigned int icode, unsigned int ifun, unsigned int valC, unsigned int valA, unsigned int valB, unsigned int dstE, unsigned int dstM, unsigned int srcA, unsigned int srcB);



#endif


#ifndef MEMORYSTAGE_H
#define MEMORYSTAGE_H
typedef struct
{
    unsigned int stat;
    unsigned int icode;
    unsigned int Cnd;
    unsigned int valE;
    unsigned int valA;
    unsigned int dstE;
    unsigned int dstM;
} mregister;

//prototypes for functions called from files other than fetchStage
mregister getMregister();
void memoryStage(forwardType * forward, statusType * status, bubbleType * bubble);
void clearMregister();
void updateMregister(unsigned int stat, unsigned int icode, unsigned int Cnd, unsigned int valE, unsigned int valA, unsigned int dstE, unsigned int dstM);

#endif


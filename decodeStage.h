#ifndef DECODESTAGE_H
#define DECODESTAGE_H

// struct for D register
typedef struct
{
    unsigned int stat;
    unsigned int icode;
    unsigned int ifun;
    unsigned int rA;
    unsigned int rB;
    unsigned int valC;
    unsigned int valP;
} dregister;

// prototypes
dregister getDregister();
void clearDregister();
void updateDregister(unsigned int stat, unsigned int icode, unsigned int ifun, unsigned int rA,
                        unsigned int rB, unsigned int valC, unsigned int valP);
void decodeStage(forwardType forward, bubbleType * b);


#endif


#ifndef FETCHSTAGE_H
#define FETCHSTAGE_H
typedef struct 
{
    unsigned int predPC;
} fregister;

//prototypes for functions called from files other than fetchStage
fregister getFregister();
void clearFregister();
void fetchStage(forwardType forward, bubbleType bubble);


#endif

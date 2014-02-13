/**
 * Main
 * The main class will handle the process 
 * to load a .yo assembly language file
 * into memory and then execute that file
 * instruction by instruction.
 * Sam Radford and Alex Fitzgerald - Team7
 */

#include <stdio.h>
#include <stdlib.h>
#include "bool.h"
#include "tools.h"
#include "forwarding.h"
#include "status.h"
#include "bubbling.h"
#include "memory.h"
#include "dump.h"
#include "loader.h"
#include "memoryStage.h"
#include "executeStage.h"
#include "decodeStage.h"
#include "fetchStage.h"
#include "writebackStage.h"


void initialize();

/**
 * Function: Main
 * This function handles the process
 * of loading and execution after
 * everything has been initialized
 */
int main(int argv, char * args[])
{
    initialize();
   
    int clockCount=0;
    bool stop = FALSE;
    forwardType forward;
    statusType status;
    bubbleType bubble;
    if(argv == 2) {
        if(!load(argv,args[1])){
            dumpMemory();
            exit(EXIT_FAILURE);
        }
        //Executes the y-86 pipeline
        //machine until there is
        //an error with the input file
        //or the input file reaches EoF
        while(stop == FALSE) {
            stop = writebackStage(&forward,&status,bubble);
            memoryStage(&forward,&status, &bubble);
            executeStage(&forward,status, &bubble);
            decodeStage(forward, &bubble);
            fetchStage(forward, bubble);
            clockCount = clockCount+1;
        }
        printf("\nTotal clock cycles = %d\n", clockCount);
    
    }

    else{
        printf("Invalid args\n");
    }

    
  
}


/**
 * Function: Initialize
 * Clears the memory
 * and all the values of the
 * registers
 */
void initialize()
{
    clearMemory();
    clearRegisters();
    clearFregister();
    clearDregister();
    clearEregister();
    clearMregister();
    clearWregister();
    initializeFuncPtrArray();
}




#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bool.h"
#include "loader.h"
#include "tools.h"

/**
 * Sam Radford and Alex Fitzgerald
 */

/**
 * load
 * This function takes a command line argument containing
 * the name of a file and loads the contents of the file
 * into memory byte by byte until there is an error
 * @param: argc: the number of args
 *         args: the name of the file
 * @return: goodLine: returns false if an error was found
 */
bool load(int argc, char *args) {
    bool goodLine = TRUE; 
    int lineNumber = 1;
    if(validFileName(args)){
        FILE * fi;
        fi = fopen(args,"r");
        char record[80];      
        int prevAdr = -1,startAdr,lineSize;
        int firstLine = 0;
        int loop;
        int dataLoc,byteLoc;
        int dataLength;
        bool lastLineData;
        bool memError;
        while(fgets(record,80,fi)!=NULL && goodLine){
            dataLoc = 9;  //Where data starts
            byteLoc = 0;  //Which byte of data to get
            if (isAddress(record)){ 
                startAdr=setLastAddress(record); //Address in a record
            }
            int lineSize = lineLength(record); //Length of record
                
            if(record[lineSize]!='\n') {
                discardRest(record,fi);
            }
            
            //Calls the checkLine
            if(checkLine(record)) {
                dataLength = lengthOfData(record); //Length of data in a record
                
                //If the record isn't the first line of the file
                if(firstLine==1) {
                   if(!(lastLineData&&startAdr>prevAdr)) { //If there was data on the last line make sure the address is bigger
                      goodLine = FALSE;
                      //printf("Error on line %d\n",lineNumber);
                   }
                }
                //If the record is the first line
                else{
                    if(isAddress(record)) {
                        firstLine = 1;
                    }
                }
            }
            
            //checkLine returns false
            else{ 
                //printf("Error on line %d\n",lineNumber);
                goodLine = FALSE;
            }
            
            //If all checks have passed
            if(goodLine){
                //Goes through the data
                for(loop = 0; loop < dataLength;loop+=2){
                    //Adds the byte to memory
                    putByte(startAdr,grabDataByte(record,byteLoc),&memError);
                    //printf("%x\n",grabDataByte(record,byteLoc));
                    startAdr++;
                    byteLoc+=2;
                }
                //puts(record); //Output record
                prevAdr = startAdr - 1;
                lineNumber++;
                lastLineData = isData(record);
           } 
           else if(goodLine == FALSE){
               printf("Error on line %d\n", lineNumber);
               puts(record);
           }
      }   
        fclose(fi);
     }
     
     
     //If the file didn't open   
     else{
         goodLine = FALSE;
         printf("Error opening file");
     }
     
     
     return goodLine;

     
}

/**
 * validFileName
 * This function checks to make sure the file name 
 * given as an argument is valid(fileName.yo,exists)
 * @param: fileName: the name of the file to check
 * @return valid: Returns true if the filename ends in .yo and the file exists
 */
bool validFileName(char* fileName) {
    bool valid = FALSE;;    
    int loop =0;
    
    while(fileName[loop] !='.' && loop < BUFFER_SIZE) {
        loop++;
    }

    //Checks to make sure it's long enough to end in .yo, that it does end in .yo
    //and that the file with the given name actually exits
    if(fileName[loop] == '.' && fileName[loop+1] == 'y' && fileName[loop+2] == 'o'){
          FILE * fi;
          fi = fopen(fileName,"r");
          if(fi != NULL){
                valid = TRUE;
                fclose(fi);
          }
          valid = TRUE;
    }
    //printf("%d\n",valid);   
    if (valid == FALSE){
        printf("File Opening Failed\n");
        printf("Correct Usage: yess <filename>.yo\n");
    }
    return valid;
}

/** grabAddress
 * This function returns the address written in a record
 * @param: dataRecord: The record to grab the address from
 * @return: retAddr: The address to return
 */

char* grabAddress(char* dataRecord) {
    char* retAddr = strndup(dataRecord+2,5); //The char from 2-5 make up the address
    return retAddr;
}

/**
 * discardRest
 * This function moves the character pointer
 * in a file until it finds a new line or an end of file
 * @param: recentLine: The most recently read line
 *         userFile: The pointer to the file being read
 */
void discardRest(char* recentLine, FILE * userFile) {
     int x = 0;//fgetc(userFile);
     while(x != '\n'&& !feof(userFile)){   
         x = fgetc(userFile); //Moves the pointer until it finds '\n' 
     }

}


/**
 * isAddress
 * This function checks to see if the record 
 * contains an address and that the address is valid
 * @param: record: The array of char to check
 * @return: valid: Returns true if the address follows the format
 *                 and the address is after the previous one.
 */
//Check that the address follows the previous 
bool isAddress(char* record){
    int loop;
    bool valid = FALSE;

    
    //Checks for correct format of 0xhexhexhex
    if(record[2]=='0'&& record[3]=='x'){ 
        if(record[7] == ':'){
           if(checkHex(record,4,6)){
                valid = TRUE;
            }
        }
    }
   
    return valid;
}

/**
 * isData
 * This function checks to see if the record
 * contains data and that the data is valid
 * @param: record: the array of char to check
 * @return: valid: returns true if there is data 
 *                 and the data is valid
 */

bool isData(char* record) {
    int loop;
    bool valid = TRUE;
    bool foundSpace = FALSE;
    int digits = 0;
    for(loop = 9; loop <= 20; loop++){
        if(record[loop] == 32){
            foundSpace == TRUE; //If there is a space no data can follow
        }
        else if(isxdigit(record[loop])){
            digits++;
            if(foundSpace){
                valid = FALSE; //If there is data after a space: False            
            }
        }
        else{
            valid = FALSE;  //If something not a space or hex is found: False
        }
    }
    if(digits % 2 != 0){
        valid = FALSE;  //Checks to make sure there is a not a 4-bit char alone
    }
    return valid;
}

/**
 * isSpaces
 * This function checks a portion of a record
 * and checks to see if every char is a space
 * @param: record: The array of char
 *         start: The starting index to check
 *         end: The last index to check
 * @return: vald: returns true if the char are all spaces
 */

bool isSpaces(char* record, int start, int end){
    int loop;
    bool valid = TRUE;
    for(loop = start; loop<=end; loop++){
        if(record[loop] != ' '){
            valid = FALSE;  //If one char isn't a space then return false
        }
    }
    return valid;
}

/**
 * checkHex
 * This function checks a portion of a record
 * and determines if every char is a hex digit
 * @param: record: The array of char
 *         start: The starting index to check from
 *         end: The last index to check
 * @return: valid: Returns true if all char are hex
 */

bool checkHex(char* record, int start, int end){
    int loop;
    bool valid = TRUE;
    for(loop = start; loop <= end; loop++){
        if(isxdigit(record[loop])==0){
            valid = FALSE;     //If the char isn't a hex then the chars aren't hex 
        }
    }

    return valid;
}

unsigned char grabDataByte(char* record, int start) {
    //Convert data into an unsigned int 
    //Pass that and start into getByteNumber in tools.c
    char* data = strndup(record+9+start,2); 
    unsigned char numString[3];
    numString[0] = data[0];
    numString[1] = data[1];
    numString[2] = 0;
    long int intVal = strtol(numString,NULL,16);
    unsigned char byte = getByteNumber(0,intVal); 
    return byte;
}

bool checkLine(char * record) {
    bool validLine = FALSE;
    if(isAddress(record)) {
        //If there is an address and there is data
        if(isData(record)){
           if(isSpaces(record,0,1) && isSpaces(record,8,8) && isSpaces(record,21,21)){
              if(record[22] == '|'){
                validLine =TRUE;
              }
           }
        }
        //If there is an address and no data
        else{
            if(isSpaces(record,0,1) && isSpaces(record,8,21) && record[22] == '|')
                validLine = TRUE;
        }
    }
    //If there is no address
    else{
        if(isSpaces(record,0,21) && record[22]== '|')
          validLine = TRUE;
    }
    return validLine;
}

/**
 * lineLength
 * This function takes a record and returns the length
 * @param: record: The record to check the length
 * @return: x: the size of the record
 */
int lineLength(char record[80]){
    int x = 0;
    while(record[x] != '\n' && x < 80){ 
        x++;                           //Determines length
    }
    return x;
}

/**
 * setLastAddress
 * returns the value of the last address that had a value
 * stored into it
 * @param: record: The record containing the address
 * @return: intVal: The value of the address
 */

int setLastAddress(char* record){
     char* address = grabAddress(record);
     unsigned char numAddr[4];
     numAddr[0] = address[2];
     numAddr[1] = address[3];
     numAddr[2] = address[4];
     numAddr[3] = 0;
     long int intVal = strtol(numAddr,NULL,16);
     return intVal;     
}

/**
 * lengthOfData
 * returns the length of the data in a record
 * @param: record: The record containing the data
 * @return: x: the length of the data
 */
     
int lengthOfData(char * record) {
    int x = 0;
    while(record[x+9]!=' ') {
        x++;
    }
    return x;
}

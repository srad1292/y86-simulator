#define BUFFER_SIZE 80


bool load(int argc,char *args);
bool validFileName(char* fileName);
void discardRest(char* recentLine,FILE * userFile);
char* grabAddress(char* dataRecord);
bool isAddress(char* record);
bool isData(char* record);
bool isSpaces(char* record,int start, int end);
bool checkHex(char* record, int start, int end);
unsigned char grabDataByte(char* record,int start);
bool checkLine();
int lineLength(char record[]);
bool spacesInAddress(char* record);
int setLastAdress(char* address);
int lengthOfData(char*  record);

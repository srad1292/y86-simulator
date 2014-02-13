#define HALT 0x0   //lab5
#define NOP  0x1   //lab5
#define SAOK 0x1   //Normal execution           taken from pg 384 in text book.
#define SHLT 0x2   //Halt
#define SADR 0x3   //Invalid Memory
#define SINS 0x4   //Invalid Instruction

#define CMOVXX  0x2 //lab8
#define IRRMOVL 0x2 //lab8
#define IIRMOVL 0x3 //lab7
#define IRMMOVL 0x4 //lab8
#define IMRMOVL 0x5 //lab8
#define IOPL    0x6 //lab7
#define IJXX    0x7
#define ICALL   0x8
#define IRET    0x9
#define IPUSHL  0xA //lab9
#define IPOPL   0XB //lab9
#define DUMP    0xC //lab5

#define RESP 4
#define RNONE 0xF
#define FNONE 0x0

#define ADDL 0
#define SUBL 1
#define ANDL 2  
#define XORL 3

#define RRMOVL 0 
#define CMOVLE 1 //X <= 0
#define CMOVL  2 //X <  0
#define CMOVE  3 //X =  0
#define CMOVNE 4 //X != 0
#define CMOVGE 5 //X >= 0
#define CMOVG  6 //X >  0

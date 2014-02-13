CC = gcc -g
B = bool.h tools.h
F = forwarding.h status.h bubbling.h

yess: dump.o fetchStage.o memoryStage.o tools.o writebackStage.o decodeStage.o memory.o registers.o executeStage.o loader.o main.o
	gcc dump.o fetchStage.o memoryStage.o tools.o writebackStage.o decodeStage.o memory.o registers.o executeStage.o loader.o main.o -o yess

dump.o: bool.h $(F) dump.h instructions.h fetchStage.h decodeStage.h executeStage.h memoryStage.h writebackStage.h registers.h memory.h 

fetchStage.o: $(B) $(F) instructions.h fetchStage.h decodeStage.h

memoryStage.o: $(B) $(F) instructions.h memoryStage.h writebackStage.h

tools.o: $(B)

writebackStage.o: $(B) $(F) instructions.h writebackStage.h memoryStage.h

decodeStage: $(B) $(F) instructions.h registers.h decodeStage.h executeStage.h 

memory.o: $(B) memory.h

registers.o: $(B) registers.h 

executeStage.o: $(B) $(F) instructions.h registers.h executeStage.h memoryStage.h

loader.o: bool.h loader.h 

main.o: $(B) $(F) memory.h dump.h loader.h memoryStage.h executeStage.h decodeStage.h fetchStage.h writebackStage.h

clean:
	rm -f *.o

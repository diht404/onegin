CC = g++

CFLAGS = -c

main: onegin.o utils.o
	$(CC) onegin.o utils.o -o main

onegin.o: onegin.cpp
	$(CC) $(CFLAGS) onegin.cpp

utils.o: utils.cpp
	$(CC) $(CFLAGS) utils.cpp

clean:
	del *.o main.exe
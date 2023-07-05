OBJS	= hashtable.o prime.o test.o
SOURCE	= hashtable.c prime.c test.c
HEADER	= hashtable.h prime.h
OUT	= test
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

hashtable.o: hashtable.c
	$(CC) $(FLAGS) hashtable.c 

prime.o: prime.c
	$(CC) $(FLAGS) prime.c 

test.o: test.c
	$(CC) $(FLAGS) test.c 

clean:
	rm -f $(OBJS) $(OUT)
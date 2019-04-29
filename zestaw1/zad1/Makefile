#start makefile
#comment

CC = gcc -Wall 
RESULTS = "raport2.txt"

#all: make static shared


static:
	$(CC) -c library.c -o library.o
	ar rcs liblibrary.a library.o
	

shared:
	$(CC) -c -fPIC  library.c
	$(CC) -shared -fPIC -o library.so library.o

smart:
	make clean
	make static
	rm -f *.o
	

clean: 
	rm -f *.o
	rm -f *.a





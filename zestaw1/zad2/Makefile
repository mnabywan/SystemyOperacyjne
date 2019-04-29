CC = gcc -Wall 
RESULTS = "raport2.txt"


static:
	$(CC) -c ../zad1/library.c -o ../zad1/library.o
	ar rcs ../zad1/liblibrary.a ../zad1/library.o
	$(CC) main.c -L../zad1 -llibrary -o program
	make test


shared:
	$(CC) -c -fPIC  ../zad1/library.c
	$(CC) -shared -fPIC -o ../zad1/library.so library.o
	
	$(CC) main.c -L../zad1  -llibrary -o program
	make test

#-Wl,-rpath=../zad1

clean: 
	rm -f *.o
	rm -f *.a

test:
	./program 1000 search_directory /home/mateusz/mownit zad1 tmp search_directory /home zad1 tmp search_directory /home a tmps remove_block 1 

CC = gcc -Wall
O0 = gcc -Wall -O0
O1 = gcc -Wall -O1
O2 = gcc -Wall -O2
Os = gcc -Wall -Os

RESULTS3A = "results3a.txt"

RESULTS3B = "results3b.txt"

make:
	$(CC) -c library.c library.o



lib_static:	
	$(CC) -c ../zad1/library.c -o ../zad1/library.o
	ar rcs ../zad1/liblibrary.a ../zad1/library.o

static:
	make lib_static
	$(CC) ./main.c -L../zad1 -llibrary -o program 
	make test

static_o0:
	make lib_static
	$(O0) ./main.c -L../zad1 -llibrary -o program 
	make testb

static_o1:
	make lib_static
	$(O1) ./main.c -L../zad1 -llibrary -o program 
	make testb

static_o2:
	make lib_static
	$(O2) ./main.c -L../zad1 -llibrary -o program 
	make testb

static_os:
	make lib_static
	$(Os) ./main.c -L../zad1 -llibrary -o program 
	make testb


lib_shared:
	$(CC) -c -fPIC  ../zad1/library.c
	$(CC) -shared -fPIC -o ../zad1/library.so library.o

shared:
	make lib_shared
	$(CC) main.c -L../zad1 -llibrary -o program
	make test

shared_o0:
	make lib_shared
	$(O0) main.c -L../zad1  -llibrary -o program
	make testb

shared_o1:
	make lib_shared
	$(O1) main.c -L../zad1  -llibrary -o program
	make testb

shared_o2:
	make lib_shared
	$(O2) main.c -L../zad1  -llibrary -o program
	make testb

shared_os:
	make lib_shared
	$(Os) main.c -L../zad1  -llibrary -o program
	make testb


dynamic:
	$(CC) -c -fPIC  ../zad1/library.c
	$(CC) -shared -fPIC -o ../zad1/library.so library.o
	$(CC) main_dynamic.c -o program -ldl
	make test

dynamic_o0:
	$(CC) -c -fPIC  ../zad1/library.c
	$(CC) -shared -fPIC -o ../zad1/library.so library.o
	$(CC) main_dynamic.c -o program -ldl
	make testb


dynamic_o1:
	$(CC) -c -fPIC  ../zad1/library.c
	$(CC) -shared -fPIC -o ../zad1/library.so library.o
	$(CC) main_dynamic.c -o program -ldl
	make testb

dynamic_o1:
	$(CC) -c -fPIC  ../zad1/library.c
	$(CC) -shared -fPIC -o ../zad1/library.so library.o
	$(CC) main_dynamic.c -o program -ldl
	make testb

dynamic_o2:
	$(CC) -c -fPIC  ../zad1/library.c
	$(CC) -shared -fPIC -o ../zad1/library.so library.o
	$(CC) main_dynamic.c -o program -ldl
	make testb


dynamic_os:
	$(CC) -c -fPIC  ../zad1/library.c
	$(CC) -shared -fPIC -o ../zad1/library.so library.o
	$(CC) main_dynamic.c -o program -ldl
	make testb

clean: 
	rm -f *.o
	rm -f *.a
	rm -f *.so

test:
	./program 1000 create_table 10000 search_directory /home/mateusz/mownit zad1 tmp_file search_directory /home/mateusz zad1  tmp_file search_directory /home a tmp_file >> $(RESULTS3A)
	./program 10 create_table 10000 search_directory /home/mateusz/mownit zad1 tmp_file search_directory /home/ zad1  tmp_file search_directory /home a tmp_file >> $(RESULTS3A)
	./program 1000 create_table 10000 search_directory /home/mateusz/mownit zad1 tmp_file search_directory /home/mateusz zad1  tmp_file search_directory /home a tmp_file >> $(RESULTS3A)
	./program 10 create_table 10000 search_directory /home/mateusz/mownit zad1 tmp_file search_directory /home/ zad1  tmp_file search_directory /home a tmp_file  >>$(RESULTS3A)
	

testb:
	./program 1000 create_table 10000 search_directory /home/mateusz/mownit zad1 tmp_file search_directory /home/mateusz zad1  tmp_file search_directory /home a tmp_file >> $(RESULTS3B)
	./program 10 create_table 10000 search_directory /home/mateusz/mownit zad1 tmp_file search_directory /home/ zad1  tmp_file search_directory /home a tmp_file >> $(RESULTS3B)
	./program 1000 create_table 10000 search_directory /home/mateusz/mownit zad1 tmp_file search_directory /home/mateusz zad1  tmp_file search_directory /home a tmp_file >> $(RESULTS3B)
	./program 10 create_table 10000 search_directory /home/mateusz/mownit zad1 tmp_file search_directory /home/ zad1  tmp_file search_directory /home a tmp_file >>$(RESULTS3B)
	
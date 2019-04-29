
static:
	gcc -Wall -c files.c -o files.o
	ar rcs libfiles.a files.o
	gcc -Wall main.c -L. -lfiles -o main
	
	
clean: 
	rm -f *.o
	rm -f *.a

generate:
	./main generate file1 1000 10 

copy_sys:
	./main generate file1 1000 100 
	./main copy file1 file2 1000 100 sys

copy_lib:
	./main generate file1 1000 100 
	./main copy file1 file2 1000 100 sys

sort_sys:
	./main generate file1 1000 100 
	./main sort file1 1000 100 sys

sort_lib:
	./main generate file1 1000 100 
	./main sort file1 1000 100 lib


test:

	for number in 1 4 512 1024 4096 8192 ; \
	do \
		./main generate tmp 1000 $$number >> wyniki.txt; \
		./main copy tmp tmp1 1000 $$number sys >> wyniki.txt ; \
		./main sort tmp 1000 $$number sys >> wyniki.txt ; \
		./main copy tmp1 tmp 1000 $$number lib >> wyniki.txt ; \
		./main sort tmp 1000 $$number lib >> wyniki.txt ; \
		./main generate tmp 5000 $$number >> wyniki.txt; \
		./main copy tmp tmp1 5000 $$number sys >> wyniki.txt ; \
		./main sort tmp 5000 $$number sys >> wyniki.txt ; \
		./main copy tmp1 tmp 5000 $$number lib >> wyniki.txt ; \
		./main sort tmp 5000 $$number lib >> wyniki.txt ; \
	done




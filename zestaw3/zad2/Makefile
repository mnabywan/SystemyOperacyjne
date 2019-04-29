make:
	mkdir archive
	touch file1
	touch file2
	gcc -Wall -o monitor monitor.c
	gcc -Wall -o tester tester.c

test:
	make
	./tester file1 1 8 30
	./tester file2 1 10 20
	./monitor lista 15 1
	./monitor lista 15 0

clean: 
	rm tester
	rm monitor
	rm file1
	rm file2
	rm -r archive
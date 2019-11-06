CC = gcc -Wall -std=gnu99

all: filter
	$(CC) main.c -o main -lpthread -lm

main:
	$(CC) generate_filter.c -o generate_filter -lpthread -lm

generate_filters:
	./generate_filter 5 filter5.txt
	./generate_filter 10 filter10.txt
	./generate_filter 20 filter20.txt
	./generate_filter 60 filter60.txt

test: 
	./main 1 block barbara.ascii.pgm ./filter5.txt ./barbara.ascii.5.1.block.pgm >> times.txt
	./main 2 block barbara.ascii.pgm ./filter5.txt ./barbara.ascii.5.2.block.pgm >> times.txt
	./main 4 block barbara.ascii.pgm ./filter5.txt ./barbara.ascii.5.4.block.pgm >> times.txt
	./main 8 block barbara.ascii.pgm ./filter5.txt ./barbara.ascii.5.8.block.pgm >> times.txt

	./main 1 block barbara.ascii.pgm ./filter10.txt ./barbara.ascii.10.1.block.pgm >> times.txt
	./main 2 block barbara.ascii.pgm ./filter10.txt ./barbara.ascii.10.2.block.pgm >> times.txt
	./main 4 block barbara.ascii.pgm ./filter10.txt ./barbara.ascii.10.4.block.pgm >> times.txt
	./main 8 block barbara.ascii.pgm ./filter10.txt ./barbara.ascii.10.8.block.pgm >> times.txt

	./main 1 block barbara.ascii.pgm ./filter20.txt ./barbara.ascii.20.1.block.pgm >> times.txt
	./main 2 block barbara.ascii.pgm ./filter20.txt ./barbara.ascii.20.2.block.pgm >> times.txt
	./main 4 block barbara.ascii.pgm ./filter20.txt ./barbara.ascii.20.4.block.pgm >> times.txt
	./main 8 block barbara.ascii.pgm ./filter20.txt ./barbara.ascii.20.8.block.pgm >> times.txt

	./main 1 block barbara.ascii.pgm ./filter60.txt ./barbara.ascii.60.1.block.pgm >> times.txt
	./main 2 block barbara.ascii.pgm ./filter60.txt ./barbara.ascii.60.2.block.pgm >> times.txt
	./main 4 block barbara.ascii.pgm ./filter60.txt ./barbara.ascii.60.4.block.pgm >> times.txt
	./main 8 block barbara.ascii.pgm ./filter60.txt ./barbara.ascii.60.8.block.pgm >> times.txt

	./main 1 interleaved barbara.ascii.pgm ./filter5.txt ./barbara.ascii.5.1.interleaved.pgm >> times.txt
	./main 2 interleaved barbara.ascii.pgm ./filter5.txt ./barbara.ascii.5.2.interleaved.pgm >> times.txt
	./main 4 interleaved barbara.ascii.pgm ./filter5.txt ./barbara.ascii.5.4.interleaved.pgm >> times.txt
	./main 8 interleaved barbara.ascii.pgm ./filter5.txt ./barbara.ascii.5.8.interleaved.pgm >> times.txt

	./main 1 interleaved barbara.ascii.pgm ./filter10.txt ./barbara.ascii.10.1.interleaved.pgm >> times.txt
	./main 2 interleaved barbara.ascii.pgm ./filter10.txt ./barbara.ascii.10.2.interleaved.pgm >> times.txt
	./main 4 interleaved barbara.ascii.pgm ./filter10.txt ./barbara.ascii.10.4.interleaved.pgm >> times.txt
	./main 8 interleaved barbara.ascii.pgm ./filter10.txt ./barbara.ascii.10.8.interleaved.pgm >> times.txt

	./main 1 interleaved barbara.ascii.pgm ./filter20.txt ./barbara.ascii.20.1.interleaved.pgm >> times.txt
	./main 2 interleaved barbara.ascii.pgm ./filter20.txt ./barbara.ascii.20.2.interleaved.pgm >> times.txt
	./main 4 interleaved barbara.ascii.pgm ./filter20.txt ./barbara.ascii.20.4.interleaved.pgm >> times.txt
	./main 8 interleaved barbara.ascii.pgm ./filter20.txt ./barbara.ascii.20.8.interleaved.pgm >> times.txt

	./main 1 interleaved barbara.ascii.pgm ./filter60.txt ./barbara.ascii.60.1.interleaved.pgm >> times.txt
	./main 2 interleaved barbara.ascii.pgm ./filter60.txt ./barbara.ascii.60.2.interleaved.pgm >> times.txt
	./main 4 interleaved barbara.ascii.pgm ./filter60.txt ./barbara.ascii.60.4.interleaved.pgm >> times.txt
	./main 8 interleaved barbara.ascii.pgm ./filter60.txt ./barbara.ascii.60.8.interleaved.pgm >> times.txt


all: prog

prog: *.c interface/*.c interface/*.h
	gcc *.c interface/*.c -lfl -pthread -o uffsdb -Wall -g	

clean:
	rm uffsdb
	rm data/ -R

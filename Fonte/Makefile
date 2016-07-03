all: release

release: *.c interface/*.c interface/*.h
	gcc *.c interface/*.c -lfl -o uffsdb -Wall -g -O0
	
debug: *.c interface/*.c interface/*.h
	gcc *.c interface/*.c -lfl -o uffsdb -Wall -g -DUFFS_DEBUG

clean:
	rm uffsdb
	rm data/ -R

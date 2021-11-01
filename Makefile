wordpairs: main.o hash.o crc64.o getWord.o
	cc -o wordpairs main.o hash.o crc64.o getWord.o
main.o: main.c hash.h
	cc -c main.c
hash.o: hash.c hash.h
	cc -c hash.c
crc64.o: crc64.c
	cc -c crc64.c
getWord.o: getWord.c
	cc -c getWord.c
clean:
	rm -f wordpairs main.o hash.o crc64.o getWord.o

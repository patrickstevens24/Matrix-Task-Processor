CC=gcc
CFLAGS=-pthread -I. -Wall -Wno-int-conversion -D_GNU_SOURCE

binaries=pcMatrix 

all: $(binaries)

pcMatrix: matrix.c pcmatrix.c tasks.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) -f $(binaries) *.o


 

CC = gcc
CFLAGS = -Wall -Wextra

all: mytoolkit.x mytimeout.x

mytoolkit.x: mytoolkit.c
	$(CC) $(CFLAGS) -o mytoolkit.x mytoolkit.c

mytimeout.x: mytimeout.c
	$(CC) $(CFLAGS) -o mytimeout.x mytimeout.c

clean:
	rm -f mytoolkit.x mytimeout.x

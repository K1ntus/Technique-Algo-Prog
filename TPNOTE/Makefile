CC = gcc
CFLAGS = -Wall -g -Wno-unused-function
LDLIBS = -lm

cor1: tp1.cor.c tp1-tools.c
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

cor2: tp2.cor.c tp2-tools.c
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

tp1: tp1.c tp1-tools.c
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

tp2: tp1.old.c tp1-tools.c
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

clean:
	rm cor1
	rm cor2
	rm tp1
	rm tp2

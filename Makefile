all: msgserv rmb

CC = gcc
CFLAGS = -g -Wall

msgserv: msgserv.o interface.o servers.o auxi.o
		$(CC) $(CFLAGS) -o msgserv msgserv.o interface.o servers.o auxi.o


msgserv.o: msgserv.c interface.h servers.h auxi.h defs.h
		$(CC) $(CFLAGS) -c msgserv.c

interface.o: interface.c interface.h servers.h auxi.h defs.h
		$(CC) $(CFLAGS) -c interface.c

servers.o: servers.c servers.h auxi.h defs.h
	$(CC) $(CFLAGS) -c servers.c

auxi.o: auxi.c auxi.h servers.h defs.h
	$(CC) $(CFLAGS) -c auxi.c


rmb: rmb.o interface1.o servers1.o auxi1.o
		$(CC) $(CFLAGS) -o rmb rmb.o interface1.o servers1.o auxi1.o

rmb.o: rmb.c interface1.h servers1.h auxi1.h defs.h
		$(CC) $(CFLAGS) -c rmb.c

interface1.o: interface1.c interface1.h servers1.h auxi1.h defs.h
		$(CC) $(CFLAGS) -c interface1.c

servers1.o: servers1.c servers1.h auxi1.h defs.h
	$(CC) $(CFLAGS) -c servers1.c

auxi1.o: auxi1.c auxi1.h defs.h
	$(CC) $(CFLAGS) -c auxi1.c

clean:
	$(RM) msgserv rmb *.o *~
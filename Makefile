#######################
# definicion de variables y constantes
#######################

CC=g++
UNAME := $(shell uname)

ifeq ($(UNAME), Linux) # equipos del laboratorio L1.02
	RCFTPOPT= -Wall
endif
ifeq ($(UNAME), SunOS) # hendrix
	RCFTPOPT= -Wall -lsocket -lnsl -lrt
endif

all: clienteTTT servidorTTT

clienteTTT: clienteTTT.o comun.o
	$(CC) $(RCFTPOPT) -o clienteTTT clienteTTT.o comun.o -g 

servidorTTT: servidorTTT.o comun.o
	$(CC) $(RCFTPOPT) -o servidorTTT servidorTTT.o comun.o -g 

clienteTTT.o: clienteTTT.cpp comun.c comun.h
	$(CC) $(RCFTPOPT) -c clienteTTT.cpp comun.c

servidorTTT.o: servidorTTT.cpp comun.c comun.h
	$(CC) $(RCFTPOPT) -c servidorTTT.cpp comun.c

clean:
	-rm -f clienteTTT servidorTTT clienteTTT.o servidorTTT.o comun.o
	

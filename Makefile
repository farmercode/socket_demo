all	: select_server client
CFLAGS=-g
select_server	: select_server.o
	cc select_server.o -o select_server

client	:	client.o
	cc client.o -o	client
clean :
	rm -f *.o
	rm -f client select_server

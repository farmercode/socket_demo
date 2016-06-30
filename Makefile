all	: select_server client poll_server epoll_server
CFLAGS=-g
select_server	: select_server.o
	cc select_server.o -o select_server

poll_server		: poll_server.o
	cc poll_server.o -o poll_server

client	:	client.o
	cc client.o -o	client
epoll_server : epoll_server.o
	cc epoll_server.o -o epoll_server

clean :
	rm -f *.o
	rm -f client select_server poll_server epoll_server

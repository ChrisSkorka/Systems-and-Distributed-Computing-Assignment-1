project: client server

client: client.c socket.c socket.h file.c file.h
	gcc -o client client.c socket.c file.c

server: server.c socket.c socket.h file.c file.h
	gcc -o server server.c socket.c file.c
	
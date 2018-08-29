project: client server

client: client.c socket.c file.c
	gcc -o client client.c

server: server.c socket.c file.c
	gcc -o server server.c
	
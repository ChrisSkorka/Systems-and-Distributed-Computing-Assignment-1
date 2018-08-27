project: command client server

command: command.c
	gcc -o command command.c

client: client.c socket.c
	gcc -o client client.c

server: server.c socket.c
	gcc -o server server.c
	
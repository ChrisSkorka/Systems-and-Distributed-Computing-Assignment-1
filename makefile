service: client.c server.c command.c
	gcc -o client client.c
	gcc -o server server.c
	gcc -o command command.c
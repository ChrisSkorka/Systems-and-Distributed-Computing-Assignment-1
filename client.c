#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <wordexp.h>

#include "socket.c"

#define PORT 46564
#define COMMAND_BUFFER_SIZE 512
#define MESSAGE_BUFFER_SIZE 1024

typedef enum { false, true } bool;

void lg(char* s){
    if(true)
        printf("Log: %s\n", s);
}

int connectToServer(char* server_ip){
    
    // variables
    int fd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    // create socket
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1){
        printf("Error opening socket\n");
        return 0;
    }

    // get host from args into server struct
    server = gethostbyname(server_ip);
    if (server == NULL) {
        printf("Error no such host\n");
        return 0;
    }

    // setup serv_addr struct for socket
    //bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
        (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);
    serv_addr.sin_port = htons(PORT);

    // connect to socket
    if (connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        printf("Error connecting\n");
        return 0;
    }

    return fd;
}

void print(char* message){
    printf("%s\n", message);

    // TODO print 40 lines at a time
}

void writeFile(char* filename, char* message, long length){
    FILE *fp;
    fp = fopen(filename, "w");
    fwrite(message, sizeof(char), length, fp);

    fclose(fp);
}

void processCommand(int argc, char** argv, char* server_ip){

    if(argc < 1){
        printf("No Command provided\n");
        return;
    }

    int socket_fd = connectToServer(server_ip);
    if(socket_fd == 0)
        return;

    char* command = argv[0];
	if(strcmp(command, "list") == 0){ // list [-l] [-f] [pathname] [localfile]

		// default argument values
		bool l = false;
		bool f = false;
		char* pathname = "";
		char* localfile = "";

		// get arguments from command line into corresponding variables
		int i = 1;
		if(i < argc && strcmp(argv[i], "-l") == 0){
			l = true;
			i++;
		}
		if(i < argc && strcmp(argv[i], "-f") == 0){
			f = true;
			i++;
		}
		if(i < argc){
			pathname = argv[i];
			i++;
		}
		if(i < argc){
			localfile = argv[i];
			i++;
		}

		// compose the command and arguments
		char sc[300] = "";
		char* server_command_args = sc;
		//strcat(shell_command, "ls");
		if(l){
			strcat(server_command_args, " -l");
		}
		if(pathname[0] != 0){
			strcat(server_command_args, " ");
			strcat(server_command_args, pathname);
		}

        int code = sendString(socket_fd, "list");
        if(code >= 0)
            code = sendString(socket_fd, server_command_args);
        if(code < 0){
            printf("Error sending request\n");
            close(socket_fd);
            return;
        }

        char* response = receiveString(socket_fd);

        if(localfile[0] == 0){ // if no local file specified (print to screen)
            print(response);
        }else{ // write response to file

        }


	}else if(strcmp(command, "get") == 0){ // get filepath [-f] [localfile]

		// default argument values
		bool f = false;
		char* filepath = "";
		char* localfile = "";

		// get arguments from command line into corresponding variables
		if(argc <= 1){
			printf("No filepath provided\n");
			return;
		}
		filepath = argv[1];

		int i = 2;
		if(i < argc && strcmp(argv[i], "-f") == 0){
			f = true;
			i++;
		}
		if(i < argc){
			localfile = argv[i];
			i++;
		}

        int code = sendString(socket_fd, "get");
        if(code >= 0)
            code = sendString(socket_fd, filepath);
        if(code < 0){
            printf("Error sending request\n");
            close(socket_fd);
            return;
        }

        long length;
        char* response = receiveArray(socket_fd, &length);

        if(localfile[0] == 0){ // if no local file specified (print to screen)
            // print(response);
            printf("%.*s\n", (int)length, response);
        }else{ // write response to file
            writeFile(localfile, response, length);
        }

	}else if(strcmp(command, "put") == 0){ // put localfile [-f] [newname]: copy file to server

	}else if(strcmp(command, "sys") == 0){ // sys

		// #ifdef _WIN32
		// SYSTEM_INFO siSysInfo;
		// GetSystemInfo(&siSysInfo);

		// printf("Hardware information: \n");  
		// printf("  OEM ID: %u\n", siSysInfo.dwOemId);
		// printf("  Number of processors: %u\n", 
		// 	siSysInfo.dwNumberOfProcessors); 
		// printf("  Page size: %u\n", siSysInfo.dwPageSize); 
		// printf("  Processor type: %u\n", siSysInfo.dwProcessorType); 
		// printf("  Minimum application address: %lx\n", 
		// 	siSysInfo.lpMinimumApplicationAddress); 
		// printf("  Maximum application address: %lx\n", 
		// 	siSysInfo.lpMaximumApplicationAddress); 
		// printf("  Active processor mask: %u\n", 
		// 	siSysInfo.dwActiveProcessorMask);
		// #endif

	}else if(strcmp(command, "delay") == 0){ // delay integer
		int delay = 0;

		// get arguments from command line into corresponding variables
		if(argc <= 1){
			printf("No delay provided\n");
            close(socket_fd);
			return;
		}
		
        // send command and delay to server
        int code = sendString(socket_fd, "delay");
        if(code >= 0)
            code = sendString(socket_fd, argv[1]);
        if(code < 0){
            printf("Error sending request\n");
            close(socket_fd);
            return;
        }

        // get and display respons
        char* response = receiveString(socket_fd);
        print(response);
        
	}else{ // unknown command
        printf("Command not recognised\n");
    }

    close(socket_fd);
    // TODO close socket
}

void main(int argc, char** argv){

    // require server name
    if(argc < 2){
        printf("IP argument required\n");
        return;
    }
    char* server_ip = argv[1];

    char cmd_buffer[COMMAND_BUFFER_SIZE];
    // char* cmd_buffer = malloc(COMMAND_BUFFER_SIZE);

    // continuous command execution loop
    while(1){
        // query for command
        printf("> ");
        bzero(cmd_buffer, COMMAND_BUFFER_SIZE);
        fgets(cmd_buffer, COMMAND_BUFFER_SIZE - 1, stdin);
        wordexp_t in_arg_struct;
        char **in_argv;
        int in_argc = 0;

        // remove new line character at the end if input
        cmd_buffer[strlen(cmd_buffer) - 1] = 0;

        // if command is exit, break loop before sending any command
        if(strcmp(cmd_buffer, "exit") == 0)
            break;

        // break arguments individual strings
        wordexp(cmd_buffer, &in_arg_struct, 0);
        in_argv = in_arg_struct.we_wordv;
        in_argc = in_arg_struct.we_wordc;
        
        // for(int i = 0; i < in_argc; i++)
        //     printf("%s\n", in_argv[i]);
        // wordfree(&cmd_arguments_struct);

        // process command
        processCommand(in_argc, in_argv, server_ip);
    }
}
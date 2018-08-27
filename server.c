#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "socket.c"
#include "file.c"

#define PORT 46564
#define MESSAGE_BUFFER_SIZE 1024

typedef enum { false, true } bool;

char* shell(char* shell_command){
    // execute command and store result, status and/or error
    FILE *command_fp;
    int status;

    // execute
    command_fp = popen(shell_command, "r");
    if(command_fp == NULL){
        return "Error: could not execute shell command";
    }

    // get length of file
    int tmp_size = 128;
    int str_max_size = 2*tmp_size;
    char* tmp = calloc(tmp_size, sizeof(char));
    char* response = calloc(str_max_size, sizeof(char));

    // read file into string
    while(fgets(tmp, tmp_size, command_fp) != NULL){

        // if buffer is full, double size
        if(strlen(response) + tmp_size + 1 >= str_max_size){
            str_max_size *= 2;
            char* new = calloc(str_max_size, sizeof(char));
            strcpy(new, response);
            free(response);
            response = new;
        }

        // append next section
        strcat(response, tmp);
    }

    return response;
}

void main(){
    // variables
    struct sockaddr_in addr;
    struct sockaddr client;
    int socket_fd, connection_socket_fd, code;

    // create socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd == -1){
        printf("Error opening socket\n");
        return;
    }

    // setup socket binding settings
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = 0;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;

    // bind socket
    if(bind(socket_fd, (struct sockaddr *) & addr, sizeof(struct sockaddr_in) ) == -1){
        printf("Error binding socket\n");
        return;
    }

    // notify if successful
    printf("Socket bound to port %i\n", PORT);

    // wait for connection
    listen(socket_fd, 5);

    while(1){
        printf("Ready for connection\n");

        // accept any connection
        int clienlen = sizeof(client);
        connection_socket_fd = accept(socket_fd, &client, &clienlen);
        if(connection_socket_fd < 0){
            printf("Error on accept\n");
            continue;
        }
        
        printf("New connection\n");
        
        // base command
        char* command = receiveString(connection_socket_fd);

        // print command and then execute it
        printf("> %s\n", command);

        // check for each command and act accordingly
        if(strcmp(command, "list") == 0){ // list args

            // get shell arguments and compose chell command string
            char* shell_args = receiveString(connection_socket_fd);
            char shell_base_command[] = "ls";
            char* shell_command = calloc(strlen(shell_base_command) + strlen(shell_args) + 2, sizeof(char));
            strcat(shell_command, shell_base_command);
            strcat(shell_command, shell_args);

            // execute shell command
            char* response = shell(shell_command);
            
            // respond
            sendString(connection_socket_fd, response);

        }else if(strcmp(command, "get") == 0){ // get filepath
        
            // get filepath
            char* filepath = receiveString(connection_socket_fd);

            long length;
            int status;
            char* file = readFile(filepath, &length, &status);
            char status_str[MESSAGE_BUFFER_SIZE];
            sprintf(status_str, "%i", status);

            sendString(connection_socket_fd, status_str); // status/read success
            sendArray(connection_socket_fd, file, length); // file content or error message 
            
        }else if(strcmp(command, "put") == 0){
            
            // get all parameters
            long length;
            char* newname = receiveString(connection_socket_fd);
            char* f_str = receiveString(connection_socket_fd);
            char* file = receiveArray(connection_socket_fd, &length);

            // decode -f parameter
            bool f = false;
            if(strcmp(f_str, "-f") == 0)
                f = true;

            // write file to disk
            char* status = writeFile(newname, file, length, f);

            if(status == NULL)
                status = "File copied successfully";

            sendString(connection_socket_fd, status);

        }else if(strcmp(command, "sys") == 0){
            
        }else if(strcmp(command, "delay") == 0){ // delay integer
            // read delay duration
            char* delay_str = receiveString(connection_socket_fd);
            int delay = atoi(delay_str);

            // sleep for specified time
            sleep(delay);

            // respond
            sendString(connection_socket_fd, delay_str);
        }

        close(connection_socket_fd);
    }
}
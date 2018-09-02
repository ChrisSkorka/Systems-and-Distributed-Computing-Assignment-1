// /////////////////////////////////////////////////////////////////////////////
// Filename:        server.c
// Author:          Christopher Skorka
// Date Created:    21/08/2018
// Description:     Server side of the remote progam. It waits for queries,
//                  processes them and responds.
// /////////////////////////////////////////////////////////////////////////////

// INCLUDES ////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "socket.h"
#include "file.h"

#if defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>
#else
    #include <sys/socket.h>
    #include <netdb.h>
    #include <netinet/in.h>
#endif

// TYPEDEFS ////////////////////////////////////////////////////////////////////
typedef enum {false, true} bool;

// PROTOTYPES //////////////////////////////////////////////////////////////////
char* shell(char* shell_command);
void main();

// FUNCTIONS ///////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// lets the operating systems shell execute a command and returns its output
// Parameters:	char* shell_command:  the command as a single string 
// Returns:		char*:                shell output or error message on faliure
// -----------------------------------------------------------------------------
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
    // since the output size is not known, a buffer will double in size when it
    // fills up and the content is copied
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

    // shell output
    return response;
}

// -----------------------------------------------------------------------------
// main function of server.c, opens and binds a socket, listens for connections
// accept connections and processes connections/requests in new processes
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void main(){
    // variables
    struct sockaddr_in addr;
    struct sockaddr client;
    int socket_fd, connection_socket_fd, code;

    // setup sockets for windows
    #if defined(_WIN32) || defined(_WIN64)
        WSADATA wsa;
        if(WSAStartup(MAKEWORD(2,2),&wsa) != 0){
            printf("Failed setting up socket for win. Error Code : %d", 
                WSAGetLastError());
            return;
        }
    #endif

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
    if(bind(socket_fd, (struct sockaddr *) & addr, 
        sizeof(struct sockaddr_in) ) == -1){
        printf("Error binding socket\n");
        return;
    }

    // notify if successful
    printf("Socket bound to port %i\n", PORT);

    // wait for connection
    listen(socket_fd, 5);

    while(1){

        // accept any connection
        int clienlen = sizeof(client);
        connection_socket_fd = accept(socket_fd, &client, &clienlen);
        if(connection_socket_fd < 0){
            printf("Error on accept\n");
            continue;
        }
        
        // fork here
        #if defined(_WIN32) || defined(_WIN64)
            // windows equivilant here
        #else
            int process = fork();
            // if child process exit listining loop and process request
            if(process == 0){
                break;
            }
        #endif

    }

    // base command
    char* command = receiveString(connection_socket_fd);

    // print command and then execute it
    printf("> %s\n", command);

    // check for each command and act accordingly
    if(strcmp(command, "list") == 0){ // list args

        // get shell arguments and compose shell command string
        char* shell_args = receiveString(connection_socket_fd);
        char* shell_base_command;
        #if defined(_WIN32) || defined(_WIN64)
            shell_base_command = "dir";
        #else
            shell_base_command = "ls";
        #endif
        
        // concatinate command and arguments into a string
        char* shell_command = calloc(strlen(shell_base_command) + 
            strlen(shell_args) + 2, sizeof(char));
        strcat(shell_command, shell_base_command);
        strcat(shell_command, shell_args);

        // execute shell command
        char* response = shell(shell_command);
        
        // respond
        sendString(connection_socket_fd, response);

    }else if(strcmp(command, "get") == 0){ // get filepath
    
        // get filepath
        char* filepath = receiveString(connection_socket_fd);

        // read specified file
        long length;
        int status;
        char* file = readFile(filepath, &length, &status);
        char status_str[MESSAGE_BUFFER_SIZE];
        sprintf(status_str, "%i", status);

        // send read success status
        sendString(connection_socket_fd, status_str); // status/read success
        if(status) // if red successfully
            sendArray(connection_socket_fd, file, length); // file content 
        else
            sendString(connection_socket_fd, file); // send error message
        
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

        // if red successfully say so
        if(status == NULL)
            status = "File copied successfully";

        // respond
        sendString(connection_socket_fd, status);

    }else if(strcmp(command, "sys") == 0){

        // strings to form the response
        char* response;
        char* os_info_header = "OS info:\n";
        char* os_info = "";
        char* cpu_info_header = "CPU info:\n";
        char* cpu_info = "";

        #if defined(_WIN32) || defined(_WIN64)
            // TODO
        #else
            os_info = shell("cat /etc/os-release | grep ID");
            cpu_info = shell("lscpu");
        #endif

        // get length of response
        long length = strlen(os_info_header) + 
            strlen(os_info) + 
            strlen(cpu_info_header) + 
            strlen(cpu_info) + 1;

        // init string
        response = malloc(length);
        response[0] = 0;
        
        // concatinate all strings
        strcat(response, os_info_header);
        strcat(response, os_info);
        strcat(response, cpu_info_header);
        strcat(response, cpu_info);

        // respond
        sendString(connection_socket_fd, response);

    }else if(strcmp(command, "delay") == 0){ // delay integer
        // read delay duration
        char* delay_str = receiveString(connection_socket_fd);

        // check if the delay provided is valid (a positive integer)
        bool is_valid = true;
        long length = strlen(delay_str);
        for(int i = 0; i < length; i++)
            is_valid &= ('0' <= delay_str[i] && delay_str[i] <= '9');

        if(is_valid){ // if valid delay
            // str to int
            int delay = atoi(delay_str);

            // sleep for specified time
            sleep(delay);

            // respond
            sendString(connection_socket_fd, delay_str);

        }else{ // if delay is invalid (eg negative or non integer)
            // respond error
            sendString(connection_socket_fd, "Error: not a positive integer");
        }
    }

    // close connection
    close(connection_socket_fd);
}

// /////////////////////////////////////////////////////////////////////////////
// END OF FILE
// /////////////////////////////////////////////////////////////////////////////
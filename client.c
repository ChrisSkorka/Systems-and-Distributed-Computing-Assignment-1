// ----------------------------------------------------------------------------
// Filename:        client.c
// Author:          Christopher Skorka
// Date Created:    21/08/2018
// Description:     Client side of the remote server program, it queries for
//                  commands, forwards them to the server and prints or saves 
//                  the response
// ----------------------------------------------------------------------------

// INCLUDES -------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "socket.h"
#include "file.h"

#if defined(_WIN32) || defined(_WIN64)
    #include <shellapi.h>
#else
    #include <wordexp.h>
#endif

// TYPEDEFS -------------------------------------------------------------------
typedef enum { false, true } bool;

typedef struct {
    bool server_responded;
    char* print_to_screen;
    char* print_to_file;
    long  print_to_file_length;
    char* localfile;
    bool override_file;
    struct timeval start_time;
    struct timeval end_time;
} ServerResponse;

// GLOBALS --------------------------------------------------------------------
#define COMMAND_BUFFER_SIZE 512

// PROTOTYPES -----------------------------------------------------------------

void print(char* original_message);
ServerResponse processList(int argc, char** argv, char* server_ip);
ServerResponse processGet(int argc, char** argv, char* server_ip);
ServerResponse processPut(int argc, char** argv, char* server_ip);
ServerResponse processSys(int argc, char** argv, char* server_ip);
ServerResponse processDelay(int argc, char** argv, char* server_ip);
void processCommand(int argc, char** argv, char* server_ip, char* plain_command);
void main(int argc, char** argv);


// FUNCTIONS ------------------------------------------------------------------

void print(char* original_message){
    // printf("\033[1A");
    // printf("%s\n", message);
    // printf("\033[1B");

    // copy string to prevent side effects from strtok
    long message_length = strlen(original_message) + 1;
    char* message = calloc(message_length, sizeof(char));
    strcpy(message, original_message);

    // variabls for spliting message into lines of 40
    int lines_at_a_time = 40;
    char* line = strtok_r(message, "\n", &message);

    // split message into blocks of 40
    while(line != NULL){
        // print up to 40 lines
        for(int i = 0; i < lines_at_a_time && line != NULL; i++){
            printf("%s\n", line);
            line = strtok_r(message, "\n", &message);
        }
        
        if(line != NULL){ // if more lines exists wait for enter
            printf("press ENTER key to continue\r");
            getchar();
            printf("\033[1A");
            printf("                           \r");
        }
    }
}

ServerResponse processList(int argc, char** argv, char* server_ip){
    // list [-l] [-f] [pathname] [localfile]
    
    // default values
    ServerResponse command_response;
    command_response.server_responded = false;
    command_response.print_to_screen = NULL;
    command_response.print_to_file = NULL;
    command_response.print_to_file_length = 0;
    command_response.localfile = NULL;
    command_response.override_file = false;

    // default argument values
    bool l = false;
    char* pathname = NULL;

    // get arguments from command line into corresponding variables
    int i = 1;
    if(i < argc && strcmp(argv[i], "-l") == 0){
        l = true;
        i++;
    }
    if(i < argc && strcmp(argv[i], "-f") == 0){
        command_response.override_file = true;
        i++;
    }
    if(i < argc){
        pathname = argv[i];
        i++;
    }
    if(i < argc){
        command_response.localfile = argv[i];
        i++;
    }

    // compose the command and arguments
    char sc[300] = "";
    char* server_command_args = sc;
    //strcat(shell_command, "ls");
    if(l){ // if -l
        strcat(server_command_args, " -l");
    }
    if(pathname != NULL){ // if pathname provided
        strcat(server_command_args, " ");
        strcat(server_command_args, pathname);
    }

    // start timer
    gettimeofday(&(command_response.start_time), NULL);

    int socket_fd = connectToServer(server_ip);
    if(socket_fd == 0)
        return command_response;

    int code = sendString(socket_fd, "list");
    if(code >= 0)
        code = sendString(socket_fd, server_command_args);
    if(code < 0){
        command_response.print_to_screen = "Error sending request";
        return command_response;
    }else{ // if request send successfully

        long length;
        char* response = receiveArray(socket_fd, &length);

        // end connetion and time it
        close(socket_fd);
        gettimeofday(&(command_response.end_time), NULL);
        command_response.server_responded = true;

        // if no local file specified (print to screen)
        if(command_response.localfile == NULL){ 
            command_response.print_to_screen = response;
        }else{ // write response to file
            command_response.print_to_file = response;
            command_response.print_to_file_length = length;
        }
    }
    
    return command_response;
}

ServerResponse processGet(int argc, char** argv, char* server_ip){
    // get filepath [-f] [localfile]

    // default values
    ServerResponse command_response;
    command_response.server_responded = false;
    command_response.print_to_screen = NULL;
    command_response.print_to_file = NULL;
    command_response.print_to_file_length = 0;
    command_response.localfile = NULL;
    command_response.override_file = false;

    // default argument values
    char* filepath = NULL;

    // get arguments from command line into corresponding variables
    if(argc <= 1){
        command_response.print_to_screen = "No filepath provided";
        return command_response;
    }
    filepath = argv[1];

    int i = 2;
    if(i < argc && strcmp(argv[i], "-f") == 0){
        command_response.override_file = true;
        i++;
    }
    if(i < argc){
        command_response.localfile = argv[i];
        i++;
    }

    // start timer
    gettimeofday(&(command_response.start_time), NULL);

    // connect to the server
    int socket_fd = connectToServer(server_ip);
    if(socket_fd == 0)
        return command_response;

    int code = sendString(socket_fd, "get");
    if(code >= 0)
        code = sendString(socket_fd, filepath);
    if(code < 0){
        command_response.print_to_screen = "Error sending request";
        return command_response;
    }else{ // if request send successfully

        long length;
        char* response_status = receiveString(socket_fd);
        char* response_file = receiveArray(socket_fd, &length);

        // end connetion and time it
        close(socket_fd);
        gettimeofday(&(command_response.end_time), NULL);
        command_response.server_responded = true;

        // if no local file specified (print to screen)
        if(command_response.localfile == NULL){ 
            command_response.print_to_screen = response_file;
        }else{ // write response to file
            if(strcmp(response_status, "1") == 0){
                command_response.print_to_file = response_file;
                command_response.print_to_file_length = length;
            }else{ // if error on server side
                command_response.print_to_screen = response_file;
            }
        }
    }

    return command_response;
}

ServerResponse processPut(int argc, char** argv, char* server_ip){
    // put localfile [-f] [newname]: copy file to server

    // default values
    ServerResponse command_response;
    command_response.server_responded = false;
    command_response.print_to_screen = NULL;
    command_response.print_to_file = NULL;
    command_response.print_to_file_length = 0;
    command_response.localfile = NULL;
    command_response.override_file = false;

    // default argument values
    char* f = "";
    char* localfile = NULL;
    char* newname = NULL;

    // get arguments from command line into corresponding variables
    if(argc <= 1){
        command_response.print_to_screen = "No localfile provided";
        return command_response;
    }
    command_response.localfile = argv[1];

    int i = 2;
    if(i < argc && strcmp(argv[i], "-f") == 0){
        f = argv[i];
        i++;
    }
    if(i < argc){
        newname = argv[i];
        i++;
    }else{
        newname = getFileName(localfile);
    }

    long length;
    int status;
    char* file = readFile(localfile, &length, &status);

    if(status){ // if file read successfully

        // start timer
        gettimeofday(&(command_response.start_time), NULL);

        // connect to the server
        int socket_fd = connectToServer(server_ip);
        if(socket_fd == 0){
            command_response.print_to_screen = "Error connecting to server";
            return command_response;
        }
        
        int code = sendString(socket_fd, "put"); // command
        if(code >= 0)
            code = sendString(socket_fd, newname); // newname
        if(code >= 0)
            code = sendString(socket_fd, f); // -f
        if(code >= 0)
            code = sendArray(socket_fd, file, length);
        if(code < 0)
            printf("Error sending request\n");
        else{ // if request send successfully

            char* response = receiveString(socket_fd);

            // end connetion and time it
            close(socket_fd);
            gettimeofday(&(command_response.end_time), NULL);

            command_response.print_to_screen = response;
            command_response.server_responded = true;

        }
    }else{ // if read failed print error message
        command_response.print_to_screen = file;
    }

    return command_response;
}

ServerResponse processSys(int argc, char** argv, char* server_ip){
    // sys

    // default values
    ServerResponse command_response;
    command_response.server_responded = false;
    command_response.print_to_screen = NULL;
    command_response.print_to_file = NULL;
    command_response.print_to_file_length = 0;
    command_response.localfile = NULL;
    command_response.override_file = false;

    // start timer
    gettimeofday(&(command_response.start_time), NULL);

    // connect to the server
    int socket_fd = connectToServer(server_ip);
    if(socket_fd == 0){
        command_response.print_to_screen = "Error connecting to server";
        return command_response;
    }

    // send command and delay to server
    int code = sendString(socket_fd, "sys");
    if(code < 0){
        command_response.print_to_screen = "Error sending request";
    }else{ // if request send successfully

        // get and display respons
        char* response = receiveString(socket_fd);

        // end connetion and time it
        close(socket_fd);
        gettimeofday(&(command_response.end_time), NULL);

        command_response.print_to_screen = response;
        command_response.server_responded = true;

    }

    return command_response;
}

ServerResponse processDelay(int argc, char** argv, char* server_ip){
    // delay integer

    // default values
    ServerResponse command_response;
    command_response.server_responded = false;
    command_response.print_to_screen = NULL;
    command_response.print_to_file = NULL;
    command_response.print_to_file_length = 0;
    command_response.localfile = NULL;
    command_response.override_file = false;

    // get arguments from command line into corresponding variables
    if(argc <= 1){
        command_response.print_to_screen = "No delay provided";
        return command_response;
    }
    
    // start timer
    gettimeofday(&(command_response.start_time), NULL);

    // connect to the server
    int socket_fd = connectToServer(server_ip);
    if(socket_fd == 0){
        command_response.print_to_screen = "Error connecting to server";
        return command_response;
    }

    // send command and delay to server
    int code = sendString(socket_fd, "delay");
    if(code >= 0)
        code = sendString(socket_fd, argv[1]);
    if(code < 0){
        command_response.print_to_screen = "Error sending request";
    }else{ // if request send successfully

        // get and display respons
        char* response = receiveString(socket_fd);

        // end connetion and time it
        close(socket_fd);
        gettimeofday(&(command_response.end_time), NULL);

        command_response.print_to_screen = response;
        command_response.server_responded = true;

    }

    return command_response;
}

void processCommand(int argc, char** argv, char* server_ip, char* plain_command){

    // default values
    ServerResponse command_response;
    command_response.server_responded = false;
    command_response.print_to_screen = NULL;
    command_response.print_to_file = NULL;
    command_response.print_to_file_length = 0;
    command_response.localfile = NULL;
    command_response.override_file = false;

    if(argc < 1){
        command_response.print_to_screen = "No Command provided";
    }else{
        // get command
        char* command = argv[0];

        // excecute command
        if(strcmp(command, "list") == 0)
            command_response = processList(argc, argv, server_ip);
        else if(strcmp(command, "get") == 0)
            command_response = processGet(argc, argv, server_ip);
        else if(strcmp(command, "put") == 0)
            command_response = processPut(argc, argv, server_ip);
        else if(strcmp(command, "sys") == 0)
            command_response = processSys(argc, argv, server_ip);
        else if(strcmp(command, "delay") == 0)
            command_response = processDelay(argc, argv, server_ip);
        else{ // unknown command
            command_response.print_to_screen = "Command not recognised";
        }
    }

    // go to start of current line
    printf("\r");

    // print either server response or error
    if(command_response.server_responded){// if server responded
    
        // calculate time taken for server to respond, end_time - start_time
        double time_elapsed = 
            (double)(command_response.end_time.tv_usec - 
                command_response.start_time.tv_usec) / 1000 +
            (double)(command_response.end_time.tv_sec - 
                command_response.start_time.tv_sec) * 1000;

        // print begining of response
        char response_time_str[128];
        sprintf(response_time_str, "Server response (%f ms):\n<%s>\n", 
            time_elapsed, plain_command);
        print(response_time_str);

        // if there is content to be printed to screen
        if(command_response.print_to_screen != NULL){
            print(command_response.print_to_screen);
        }

        // if there is content to be written to file
        if(command_response.print_to_file != NULL){
            char* error = writeFile(
                command_response.localfile, 
                command_response.print_to_file, 
                command_response.print_to_file_length, 
                command_response.override_file
            );
            if(error != NULL)
                print(error);
        }

    // if some error occured before sending the request and error is provided
    }else if(command_response.print_to_screen != NULL){ 
        print(command_response.print_to_screen);
    }

    // for next command
    printf("\n> ");
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

    printf("----------------------------------------\n");
    printf("| Client is ready, commands are        |\n");
    printf("| executed in the background           |\n");
    printf("| Commands: list, get, put, sys, delay |\n");
    printf("----------------------------------------\n");

    // continuous command execution loop
    while(1){
        // query for command
        printf("> ");
        // bzero(cmd_buffer, COMMAND_BUFFER_SIZE);
        fgets(cmd_buffer, COMMAND_BUFFER_SIZE - 1, stdin);

        // remove new line character at the end if input
        cmd_buffer[strlen(cmd_buffer) - 1] = 0;

        // if command is exit, break loop before sending any command
        if(strcmp(cmd_buffer, "exit") == 0)
            return;

        // fork here
        #if defined(_WIN32) || defined(_WIN64)
            // windows equivilant here
        #else
            int process = fork();
            if(process == 0){
                break;
            }
        #endif
    }

    // variables for split arguments
    char** in_argv;
    int in_argc = 0;

    // break arguments individual strings
    #if defined(_WIN32) || defined(_WIN64)
        in_argv = (char**)CommandLineToArgvW((LPCWSTR)cmd_buffer, &in_argc);
    #else
        wordexp_t in_arg_struct;
        wordexp(cmd_buffer, &in_arg_struct, 0);
        in_argv = in_arg_struct.we_wordv;
        in_argc = in_arg_struct.we_wordc;
    #endif

    // process command
    processCommand(in_argc, in_argv, server_ip, cmd_buffer);
}

// ----------------------------------------------------------------------------
// END OF FILE
// ----------------------------------------------------------------------------
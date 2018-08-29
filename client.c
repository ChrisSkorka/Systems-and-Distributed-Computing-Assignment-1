#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "socket.c"
#include "file.c"

#if defined(_WIN32) || defined(_WIN64)
    #include <shellapi.h>
#else
    #include <wordexp.h>
#endif

#define COMMAND_BUFFER_SIZE 512

typedef enum { false, true } bool;

void lg(char* s){
    if(true)
        printf("Log: %s\n", s);
}

void print(char* message){
    printf("%s\n", message);

    // TODO print 40 lines at a time
}

void processCommand(int argc, char** argv, char* server_ip){

    if(argc < 1){
        printf("No Command provided\n");
        return;
    }

    int socket_fd = connectToServer(server_ip);
    if(socket_fd == 0)
        return;

    char* print_to_screen = NULL;
    char* print_to_file = NULL;
    long  print_to_file_length = 0;
    char* localfile = NULL;
    bool override_file = false;
    bool server_responded = false;

    // setup timing related values
    struct timeval  start_time, end_time;

    // get command
    char* command = argv[0];
	if(strcmp(command, "list") == 0){ // list [-l] [-f] [pathname] [localfile]

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
			override_file = true;
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
		if(l){ // if -l
			strcat(server_command_args, " -l");
		}
		if(pathname != NULL){ // if pathname provided
			strcat(server_command_args, " ");
			strcat(server_command_args, pathname);
		}

        // start timer
        gettimeofday(&start_time, NULL);

        int code = sendString(socket_fd, "list");
        if(code >= 0)
            code = sendString(socket_fd, server_command_args);
        if(code < 0){
            printf("Error sending request\n");
        }else{ // if request send successfully

            long length;
            char* response = receiveArray(socket_fd, &length);

            // time end of transmission
            gettimeofday(&end_time, NULL);
            server_responded = true;

            if(localfile == NULL){ // if no local file specified (print to screen)
                print_to_screen = response;
            }else{ // write response to file
                print_to_file = response;
                print_to_file_length = length;
            }
        }

	}else if(strcmp(command, "get") == 0){ // get filepath [-f] [localfile]

		// default argument values
		char* filepath = NULL;

		// get arguments from command line into corresponding variables
		if(argc <= 1){
			printf("No filepath provided\n");
            close(socket_fd);
			return;
		}
		filepath = argv[1];

		int i = 2;
		if(i < argc && strcmp(argv[i], "-f") == 0){
			override_file = true;
			i++;
		}
		if(i < argc){
			localfile = argv[i];
			i++;
		}

        // start timer
        gettimeofday(&start_time, NULL);

        int code = sendString(socket_fd, "get");
        if(code >= 0)
            code = sendString(socket_fd, filepath);
        if(code < 0){
            printf("Error sending request\n");
        }else{ // if request send successfully

            long length;
            char* response_status = receiveString(socket_fd);
            char* response_file = receiveArray(socket_fd, &length);

            // time end of transmission
            gettimeofday(&end_time, NULL);
            server_responded = true;

            // save current time
            //gettimeofday(&end_time, NULL);

            if(localfile == NULL){ // if no local file specified (print to screen)
                print_to_screen = response_file;
            }else{ // write response to file
                if(strcmp(response_status, "1") == 0){
                    print_to_file = response_file;
                    print_to_file_length = length;
                }else{ // if error on server side
                    print_to_screen = response_file;
                }
            }
        }

	}else if(strcmp(command, "put") == 0){ // put localfile [-f] [newname]: copy file to server

		// default argument values
		char* f = "";
		char* localfile = NULL;
		char* newname = NULL;

		// get arguments from command line into corresponding variables
		if(argc <= 1){
			printf("No localfile provided\n");
            close(socket_fd);
			return;
		}
		localfile = argv[1];

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
            gettimeofday(&start_time, NULL);
            
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

                // time end of transmission
                gettimeofday(&end_time, NULL);

                print_to_screen = response;
                server_responded = true;

            }
        }else{ // if read failed print error message
            print_to_screen = file;
        }

	}else if(strcmp(command, "sys") == 0){ // sys

        // start timer
        gettimeofday(&start_time, NULL);

        // send command and delay to server
        int code = sendString(socket_fd, "sys");
        if(code < 0){
            printf("Error sending request\n");
        }else{ // if request send successfully

            // get and display respons
            char* response = receiveString(socket_fd);

            // time end of transmission
            gettimeofday(&end_time, NULL);

            print_to_screen = response;
            server_responded = true;

        }

	}else if(strcmp(command, "delay") == 0){ // delay integer

		// get arguments from command line into corresponding variables
		if(argc <= 1){
			printf("No delay provided\n");
            close(socket_fd);
			return;
		}
		
        // start timer
        gettimeofday(&start_time, NULL);

        // send command and delay to server
        int code = sendString(socket_fd, "delay");
        if(code >= 0)
            code = sendString(socket_fd, argv[1]);
        if(code < 0){
            printf("Error sending request\n");
        }else{ // if request send successfully

            // get and display respons
            char* response = receiveString(socket_fd);

            // time end of transmission
            gettimeofday(&end_time, NULL);

            print_to_screen = response;
            server_responded = true;

        }
        
	}else{ // unknown command
        print_to_screen = "Command not recognised";
    }
    

    double time_elapsed = (double) (end_time.tv_usec - start_time.tv_usec) / 1000 +
        (double) (end_time.tv_sec - start_time.tv_sec) * 1000;

    

    if(server_responded){
        printf ("Server response (%f ms):\n", time_elapsed);

        // if there is content to be printed to screen
        if(print_to_screen != NULL){
            print(print_to_screen);
        }

        // if there is content to be written to file
        if(print_to_file != NULL){
            char* error = writeFile(localfile, print_to_file, print_to_file_length, override_file);
            if(error != NULL)
                printf("%s\n", error);
        }

    }else{ // if some error occured before sending the request
        printf("%s\n", print_to_screen);
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
        // bzero(cmd_buffer, COMMAND_BUFFER_SIZE);
        fgets(cmd_buffer, COMMAND_BUFFER_SIZE - 1, stdin);
        char** in_argv;
        int in_argc = 0;

        // remove new line character at the end if input
        cmd_buffer[strlen(cmd_buffer) - 1] = 0;

        // if command is exit, break loop before sending any command
        if(strcmp(cmd_buffer, "exit") == 0)
            break;

        // break arguments individual strings
        #if defined(_WIN32) || defined(_WIN64)
            in_argv = (char**)CommandLineToArgvW((LPCWSTR)cmd_buffer, &in_argc);
        #else
            wordexp_t in_arg_struct;
            wordexp(cmd_buffer, &in_arg_struct, 0);
            in_argv = in_arg_struct.we_wordv;
            in_argc = in_arg_struct.we_wordc;
        #endif
        
        // for(int i = 0; i < in_argc; i++)
        //     printf("%s\n", in_argv[i]);
        // wordfree(&cmd_arguments_struct);

        // process command
        processCommand(in_argc, in_argv, server_ip);
    }
}
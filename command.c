#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifdef _WIN32
	#include <windows.h>
#endif

typedef enum { false, true } bool;

void main(int argc, char** argv){

	if(argc <= 1){
		printf("No command provided\n");
		return;
	}

	char* command = argv[1];

	if(strcmp(command, "list") == 0){ // list [-l] [-f] [pathname] [localfile]

		// default argument values
		bool l = false;
		bool f = false;
		char* pathname = "";
		char* localfile = "";

		// get arguments from command line into corresponding variables
		int i = 2;
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

		// compose the command
		char sc[300] = "";
		char* shell_command = sc;
		strcat(shell_command, "ls");
		if(l){
			strcat(shell_command, " -l");
		}
		if(pathname[0] != NULL){
			strcat(shell_command, " ");
			strcat(shell_command, pathname);
		}

		// execute command and store result, status and/or error
		// system(shell_command);
		FILE *command_fp;
		int status;
		char part[100];
		int len = 100;

		command_fp = popen(shell_command, "r");
		if(command_fp == NULL){
			printf("Unknown error 0x01\n");
			return;
		}

		while(fgets(part, len, command_fp) != NULL)
			printf("%s", part);
		
		status = pclose(command_fp);
		if(status == -1) {
			printf("Failed to execute command\n");
		}else{
			
		}

		//printf("list %i %i %s %s\n", l, f, pathname, localfile);

	}else if(strcmp(command, "get") == 0){ // get filepath [-f] [localfile]

		// default argument values
		bool f = false;
		char* filepath = "";
		char* localfile = "";

		// get arguments from command line into corresponding variables
		if(argc <= 2){
			printf("No filepath provided\n");
			return;
		}
		filepath = argv[2];

		int i = 3;
		if(i < argc && strcmp(argv[i], "-f") == 0){
			f = true;
			i++;
		}
		if(i < argc){
			localfile = argv[i];
			i++;
		}

		//

	}else if(strcmp(command, "put") == 0){ // put localfile [-f] [newname]: copy file to server

	}else if(strcmp(command, "sys") == 0){ // sys

		#ifdef _WIN32
		SYSTEM_INFO siSysInfo;
		GetSystemInfo(&siSysInfo);

		printf("Hardware information: \n");  
		printf("  OEM ID: %u\n", siSysInfo.dwOemId);
		printf("  Number of processors: %u\n", 
			siSysInfo.dwNumberOfProcessors); 
		printf("  Page size: %u\n", siSysInfo.dwPageSize); 
		printf("  Processor type: %u\n", siSysInfo.dwProcessorType); 
		printf("  Minimum application address: %lx\n", 
			siSysInfo.lpMinimumApplicationAddress); 
		printf("  Maximum application address: %lx\n", 
			siSysInfo.lpMaximumApplicationAddress); 
		printf("  Active processor mask: %u\n", 
			siSysInfo.dwActiveProcessorMask);
		#endif

	}else if(strcmp(command, "delay") == 0){ // delay integer
		int delay = 0;

		// get arguments from command line into corresponding variables
		if(argc <= 2){
			printf("No delay provided\n");
			return;
		}
		delay = atoi(argv[2]);

		sleep(delay);
	}

	// printf("> %s\n", command);
}
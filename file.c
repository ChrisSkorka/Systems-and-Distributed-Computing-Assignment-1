#include "file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>

// read content from file, returns content or error message
char* readFile(char* filepath, long* len, int* status){
	*status = 0; // status is failed unless success

	// if file isnt available
	if(access(filepath, F_OK) == -1){
		char* error = "Error: file doesn't exists";
		*len = strlen(error) + 1;
		return error;
	}
	
	// open file
	char* buffer = 0;
	long length;
	FILE* fp = fopen (filepath, "r");

	if(fp){
		// find length of content
		fseek (fp, 0, SEEK_END);
		length = ftell (fp);
		*len = length;
		fseek (fp, 0, SEEK_SET);

		// read from file into buffer
		buffer = malloc(length);
		if(buffer){
			fread(buffer, sizeof(char), length, fp);
		}

		fclose (fp);
		*status = 1;
		return buffer;
	}else{
		char* error = "Error: unable to open file";
		*len = strlen(error) + 1;
		return error;
	}
}

// write char array of given length to file, returns error message
char* writeFile(char* filename, char* message, long length, int override){
	if(!override && access(filename, F_OK) == 0){
		return "Error: file already exists, use -f to override files";
	}

    FILE *fp;
    fp = fopen(filename, "w");

	if(fp){
		fwrite(message, sizeof(char), length, fp);
		fclose(fp);
		return NULL;
	}else{
		return "Error: unable to open file";
	}
}

// get file name from path
char* getFileName(char* path){
	return basename(path);
}
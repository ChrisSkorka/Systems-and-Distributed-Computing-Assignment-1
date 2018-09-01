#ifndef FILE_H
#define FILE_H

// read content from file, returns content or error message
char* readFile(char* filepath, long* len, int* status);

// write char array of given length to file, returns error message
char* writeFile(char* filename, char* message, long length, int override);

// get file name from path
char* getFileName(char* path);

#endif
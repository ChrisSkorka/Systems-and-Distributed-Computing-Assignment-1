# Systems-and-Distributed-Computing-Assignment-1
Assignment 1 for Systems and Distributing computing, remote server program to execute simple remote commands

Requiements
- Data flow
  - stdin > client > request > server > process > response > client > print/write file > repeat

- Server
  - New porcess for each client
  - Return errors for unknown command
  - Return system error string to execution errors
  - No zombie processes
- Client
  - Server port: 80
  - Server ip: argument
  - New process for each command
  - 'quit' terminates client
  - client reports time taken to reponse
  - Response can be redirected using |

- Commands
  - list [-l] [-f] [pathname] [localfile]: list the files in directory
      - If no localfile specified print 40 lines at a time
    - -l:         list size, owner, creation date, access permission
    - pathname:   relative or absolute path to list files of, default is current
    - localfile:  local file to write list to
    - -f: force override local file
  - get filepath [-f] [localfile]: retrieve file from server
      - If no localfile specified print 40 lines at a time
      - If file exists error before sending request
    - filepath:   server file to get
    - localfile:  response is written to file instead of printed
    - -f:         force override local file
  - put localfile [-f] [newname]: copy file to server
    - localfile:  local file to be copied
    - -f:         override file if already exists
    - newname:    name of server file
  - sys: name and version of OS and CPU type
  - delay integer: wait interger seconds and then retrurn integer

- marking criteria
  - ✓ Server address as a command line argument
  - ✓ Client reports time taken for server to respond
  - ✓ Client runs continously in a loop but is non-blocking
  - Server spawns new process for new requests on Win32
  - ✓ Server spawns new process for new requests on Unix
  - Compiles and runs on both Win32 and Unix
  - ✓ Correct operation of list -l command on Unix
  - Correct operation of list -l command on Win32
  - ✓ Correct operation of get command file data correctly copied to screen or file as needed
  - ✓ Correct operation of put command file data correctly copied
  - ✓ Correct operation of sys command on Unix
  - Correct operation of sys command on Win32
  - Scrolling list & get that pauses after 40 lines
  - ✓ Put command create remote file with same name or specified name or return error
  - ✓ Correct operation of the -f option on list, get and put commands
  - ✓ Commands accept relative and absolute path names for local and remote paths
  - ✓ Correct operation of delay command
  - Correct operation of output redirection on Win32
  - Correct operation of output redirection on Unix
  - Server error handling on Win32 and Unix
  - Zombie removal
  - Documentation
  - Code style, completeness, robust, quality
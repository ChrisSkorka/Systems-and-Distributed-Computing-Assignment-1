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
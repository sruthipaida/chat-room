# chat-room
**Problem-Statement**:  Implementation of a server in a client/server model using Linux Stream sockets
Which consists of a server that implement a chat service using simple text protocol that clients 
use to message other clients on the service through telnet message.

PROGRAM FILE NAME – “prog3svr.c”

**DESCRIPTION**: A C program to support the server in a client/server model using LINUX 
stream sockets. The program will consist of a server that will implement a service (like a chat 
room). using simple text protocol that clients can use through telnet to message to other clients 
on the service. We have implemented this project using threads, for every client that is 
connected will be a separate thread to execute their requirements. To perform JOIN, LIST, 
MESG, BCST user must registered that means their record must be in the database. If they are 
not registered, we request them to register to perform the operations. For QUIT operation 
regardless of user registered or not the user can perform QUIT. If the user is registered and 
performs QUIT we need to remove its entry from the database. JOIN command is used to 
registered into database for this we need username of the specified client. LIST command will 
display the information of all the registered clients. MESG command is used to send message 
from one client to other. BCST command is used to send broadcast message to all the registered 
users.

**STEPS TO EXECUTE THE FILE**:
1. Open the file prog3svr.c to execute and run the file. So that we can get the output.
2. For compiling the program prog3svr.c we need to type the following command:
SERVER SIDE COMMAND: gcc prog2svr.c -lpthread
3. For Running the prog2svr.c program we need to type the following command in different 
putty's:
SERVER SIDE RUN COMMAND: ./a.out <port_number> 
CLIENT-SIDE RUN COMMAND: telnet <server name> <port_number>

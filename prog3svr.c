/*
Name: Sruthi Paida
ID : 11372690
EUID: sp1084
Instructor: Dr. Mark A. Thompson, Sr.
Email address: sruthipaida@my.unt.edu
Date : 04/19/2020
Course : CSCE 5580(Computer Networks)
Description:A C program to support the server in a client/server model using LINUX stream sockets.
			The program will consist of a server that will implement a service (like a chat room).
			using simple text protocol that clients can use through telnet to message to other
			clients on the service. We have implemented this project using threads, for every client
			that is connected will be a separate thread to execute their requirements.To peform 
			JOIN,LIST,MESG,BCST user must registered that means their record must be in the database.
			If they are not registered we request them to register to perform the operations.
			For QUIT operation regardless of user registered or not the user can perform QUIT.
			If the user is registered and performs QUIT we need to remove its entry from the database.
			JOIN command is used to registered into database for this we need user name of the specified client.
			LIST command will display the information of all the registered clients
			MESG command is used to send message from one client to other.
			BCST command is used to send broadcast message to all the registered users.
			*/

/*Required header files*/
#include<stdio.h>
#include<string.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<stdlib.h>


#define max_clients 10  //defining the maximum value client database can hold.

int clients_joined=0; // variable to maintain the count of clients joined.


struct client_variable  // struct variable to store the clients information.
{	
	int socket_id;      //variable to store the socket descriptor value of the client
    char client_name[100];  //variable to store client name.
};


struct client_variable *ClientsList[max_clients]; //struct variable to store clients information which is used as database.


void *thread_handler(void *arg);  //declaration of thread function.

int main ( int argInput, char **argc)
{
	int socketfd;     //variable to store socket descriptor value of the client
	int cli_socket;   // variable to store output of accept method
	int port_id;      //variable to store port number of the server
	int i=0;          //variable used for loops or if condition
	int bind_op;      //variable to store the output of bind function
	int listen_op;    //variable to store the output of listen method.
	struct sockaddr_in sevr_adrs; //declaring server address  variable for socket.
	struct sockaddr_in cli_adrs; //declaring client address  variable for socket.
	int cli_len;           //variable to store client address length.
	pthread_t cli_td[50];   // pthread variable declaration which used to store the thread id.
	
	/*checking the if required number of command line arguments given*/
	if (argInput != 2)
    {
        printf("usage:./prog3svr <svr_port>\n");
        exit(1);
    }
	/*creation of socekt file descriptor for UDP socket */
	socketfd=socket(AF_INET,SOCK_STREAM,0);
	
	/*Checking if the socket is created successfully */	
	if(socketfd==-1)
	{
		printf("\n : Connection Failed could not create socket");
		exit(1);
	}
	
	port_id = atoi(argc[1]);
	
	// Configure settings of the server address struct
	// Address family = Internet 
  
	bzero((char*)&sevr_adrs, sizeof(sevr_adrs));
    sevr_adrs.sin_family = AF_INET;
    sevr_adrs.sin_addr.s_addr = htonl(INADDR_ANY); //Set port number, using htons function to use proper byte order 
    sevr_adrs.sin_port=htons(port_id); //Set IP address to localhost
	
	cli_len=sizeof(cli_adrs);
    /*function to assign local port address to a socket*/
	bind_op= bind(socketfd,(struct sockaddr *)&sevr_adrs, sizeof(sevr_adrs));
	
	if(bind_op == -1)
	{
		printf("\n Error:Connection Failed could not bind address with socket \n ");
		exit(1);		
	}
	
	cli_len=sizeof(cli_adrs);
	//Listen on the socket, with 10 max connection requests queued 
	listen_op=listen(socketfd,10);
	
	if(listen_op<0)   
	{
		printf("Error on listen\n");
		exit(1);
	}
	/*Server waiting for client connections*/
	printf("Waiting for Incoming Connections...\n");
	
	
	//Accept call creates a new socket for the incoming connection
	while( (cli_socket = accept(socketfd, (struct sockaddr *)&cli_adrs, &cli_len)) )
	{
		
		struct client_variable *cli = (struct client_variable *)malloc(sizeof(struct client_variable));
		
		cli -> socket_id = cli_socket;
		
		printf("Client (%d): Connection Accepted\n",cli -> socket_id);
		
		//for each client request creates a thread and assign the client request to it to process
       //so the main thread can entertain next request
		if(pthread_create(&cli_td[i++], NULL ,&thread_handler, (void *)cli ) != 0)
		{
			printf("could not create thread\n");
			exit(1);
		}	
		
		
	}
	//closing the threads
	while(i!=0)  
	{
		pthread_join(cli_td[i],NULL);
		i--;
		
	}

	return 0;
}





void *thread_handler(void *arg)  // thread handler function
{
	struct client_variable *cli1 = (struct client_variable *)arg;  //storing the clients info in a struct variable mentioned above.
	
	printf("Client (%d): Connection Handler Assigned\n",cli1->socket_id);
	
	while(1)  //making the server to listen to client 
	{
		//variables for looping and for string extraction from the client message.
		char message1[4096];
		int s=0;
		int n=0;
		int m=0;
		char string[100] = {'\0'};
		
		char cliname[100] = {'\0'};
		char climsg[2048] = {'\0'};
		char cli_bmsg[2048] = {'\0'};
		int already_joined=0;
		
		//receiving query from the client.
		int recv_len = recv(cli1->socket_id, message1, 4096 ,0);
		message1[recv_len] = '\0';
		strncpy(string,message1,4);
		m=strlen(message1);
		
		// based on the query "JOIN" "LIST" "MESG" "BCST" "QUIT" extracting the required information from the message received from the server.
		if(strncmp(string,"JOIN",4)==0)
		{
			s=1;
			
			if(message1[4]==' ')
			{
				int i=5;
				while( message1[i]!='\0'&& i<m-1)  
				{	
					char user[10]={'\0'};
					sprintf(user,"%c",message1[i]); 
					strcat(cliname,user);
					i++;
				}
				cliname[strlen(cliname)]='\0';
			}
		}
		else if(strncmp(string,"LIST",4)==0)
		{
			s=2;
		}
		else if(strncmp(string,"MESG",4)==0)
		{
			s=3;
			int m=strlen(message1);
			int i=0;
			if(message1[4] ==' ')
			{
				int k=5;
				while( message1[k]!=' ')
				{	
					char user[10]={'\0'};
					sprintf(user,"%c",message1[k]); 
					strcat(cliname,user);
					k++;
				}
				
				i=k;

				if(message1[i]==' ')
				{	
					i++;
					while( message1[i]!='\0'&& i<m-1) 
					{	
						char mesg1[10]={'\0'};
						sprintf(mesg1,"%c",message1[i]); 
						strcat(climsg,mesg1);
						i++;
					}
				}
			}
		}
			
		else if (strncmp(string,"BCST",4)==0)
		{
			int m=strlen(message1);
			s=4;
			if(message1[4]==' ')
			{
				int j=5;
				while( message1[j]!='\0'&& j<m-1)  
				{	
					char msg1[10]={'\0'};
					sprintf(msg1,"%c",message1[j]); 
					strcat(cli_bmsg,msg1);
					j++;
				}
			}
		}
		else if (strncmp(string,"QUIT",4)==0)
		{
			s=5;
		}
		
		
		//switch case
		switch(s)
		{
	
			case 1: //for join operation based on client joined we add client to database accroding to number of clients it supports.
					
					
					
					n=strlen(cli1->client_name);
					if(n==0)
					{
						int n1=0;
						n1=strlen(cliname);
						clients_joined++;
						if( clients_joined > max_clients) // if the limit exceeds.
						{
							printf("Client (%d): Database Full. Disconnecting User\n",cli1->socket_id);  
							printf("Error: Too Many Clients Connected\n");
							send(cli1->socket_id,"Too Many Users. Disconnecting User.\n",strlen("Too Many Users. Disconnecting User.\n"),0); //sending status to client 
							close(cli1->socket_id); //closing the socket connection
							clients_joined--;  //decreasing the count of clients joined.
							cli1=NULL; //freeing up the cli1 struct memory
				
							pthread_exit(NULL);
						}
						
						int a=0;
						int i=0;
						
						while(a<max_clients)
						{		
					
								if(ClientsList[a])
								{	
									if(strncmp(ClientsList[a]->client_name,cliname,strlen(ClientsList[a]->client_name))==0 && strncmp(ClientsList[a]->client_name,cliname,strlen(cliname)-1)==0)  
									{
										already_joined=1;
										
										break;
									}
								}
								
								a++;
						}
						
						
						
					
						//if client name not null
						if (n1!=0 && already_joined==0 ) // for non register client, adding client entry to database.
						{	 
							int i=0;
							strncpy(cli1->client_name,cliname,strlen(cliname)-1);  
							
							while(i<max_clients)
							{
								if (!ClientsList[i])  
								{
									ClientsList[i] = cli1;
									break;
								}
								i++;
							}
							printf("Client (%d): JOIN %s\n",cli1->socket_id,cli1->client_name);  
							strncpy(message1,message1,sizeof(message1)-1);
							strcat(message1," Request Accepted\n");
							send(cli1->socket_id,message1,strlen(message1),0);  //sending status message client
						}
						else  // if the submision of query is done improperly
						{
							if(already_joined==1)
							{
								printf("Client (%d): User Already Registered. Discarding JOIN\n",cli1->socket_id);
								char fd[50];
								char msg[1000]={'\0'};
								int i=0;
								while(i<max_clients)  
								{
									if(ClientsList[i])
									{
										if (strncmp(ClientsList[i]->client_name,cliname,strlen(ClientsList[i]->client_name))==0)  
										{
											sprintf(fd,"%d", ClientsList[i]->socket_id);
											break;
										}
									}
									i++;
							}
							strcpy(msg,"User Already Registered : Username - ");
							strncat(msg,cliname,strlen(cliname)-1);
							strcat(msg," FD - ");
							strcat(msg,fd);
							strcat(msg,"\n");
							send(cli1->socket_id,msg,strlen(msg),0); //sending status message to server.
							memset(msg,0,1000);
							}
						
							else
							{
								
								printf("Client (%d): Unrecognizable Message. Discarding UNKNOWN Message\n",cli1->socket_id);  //printing status message on server side  
								n=strlen(cli1->client_name);
								if(n!=0)   
									send(cli1->socket_id,"Unknown Message. Discarding UNKNOWN Message.\n",strlen("Unknown Message. Discarding UNKNOWN Message.\n") ,0); //sending status message to client.
								else 
									send(cli1->socket_id,"Unregistered User. Use \"JOIN <username>\" to Register.\n",strlen("Unregistered User. Use \"JOIN <username>\" to Register.\n"), 0);  //sending status message to client.
							}
						}
					}
					else // for registered client.
					{	
						printf("Client (%d): User Already Registered. Discarding JOIN\n",cli1->socket_id);
						char fd[50];
						char msg[1000]={'\0'};
						int i=0;
						while(i<max_clients)  
						{
							if(ClientsList[i])
							{
								if (strncmp(ClientsList[i]->client_name,cliname,strlen(ClientsList[i]->client_name))==0)  
								{
									sprintf(fd,"%d", ClientsList[i]->socket_id);
									break;
								}
							}
							i++;
						}
						strcpy(msg,"User Already Registered : Username - ");
						strcat(msg,cli1->client_name);
						strcat(msg,"\t,FD - ");
						strcat(msg,fd);
						strcat(msg,"\n");
						send(cli1->socket_id,msg,strlen(msg),0); //sending status message to server.
						memset(msg,0,1000);
					}
					break;
		

			case 2:  //For  query LIST operation.
					printf("Client (%d): LIST\n",cli1->socket_id);
					n=strlen(cli1->client_name);
					if(n==0) 
					{
						printf("Unable to Locate Client (%d) in Database. Discarding LIST\n",cli1->socket_id);    
						send(cli1->socket_id,"Unregistered User. Use \"JOIN <username>\" to Register.\n",strlen("Unregistered User. Use \"JOIN <username>\" to Register.\n"), 0); //sending status message to server
					}
					//sending the information of the records in the database to the client.
					else							
					{
						char msg[1000]="USERNAME\tFD\n----------------------------\n";
						//extracting the client records from the data base.
						int i=0;
						while(i<max_clients)  
						{
							if(ClientsList[i])
							{
								char fd[50];
								strcat(msg,ClientsList[i]->client_name);
								strcat(msg,"		");
								sprintf(fd,"%d",ClientsList[i]->socket_id);
								strcat(msg,fd);
								strcat(msg,"\n");
							}
							i++;
						}	
						strcat(msg,"----------------------------\n");
						send(cli1->socket_id,msg,strlen(msg),0);//sending status message to client.  
						memset(msg,0,1000);
					}
					break;
					
			case 3: //For query MESG to send message to recipient client by one of the clients in the database 
					n=strlen(cli1->client_name);
				
					if(n==0)
					{
						printf("Unable to Locate Client (%d) in Database. Discarding MESG\n",cli1->socket_id);    // server status msessage
						
						send(cli1->socket_id,"Unregistered User. Use \"JOIN <username>\" to Register.\n",strlen("Unregistered User. Use \"JOIN <username>\" to Register.\n"), 0); // sending status msessage to client
						
					}
				
					else
					{	
						int recipients_socket=0;
						int recipient=0;;    
						int i=0;
						int j=0;
						int k=0;
						while(i<max_clients)  
						{
							if(ClientsList[i])
							{
							//checking if the recipient is registered in the database.
								if(strncmp(ClientsList[i]->client_name,cliname,strlen(cliname)) == 0 && strncmp(ClientsList[i]->client_name,cliname,strlen(ClientsList[i]->client_name)) == 0) 
								{
									recipients_socket = ClientsList[i]->socket_id;
									recipient=1;  
									break;
								}
							}
							i++;
						}
					
						k=strlen(climsg);
						if(strcmp(cli1->client_name,cliname)!=0) // checking if the sender is not sending message to himself
						{
							if(k!=0)
							{	
								if(recipient==1) 
								{	
									char msg[4096]={'\0'};
									char msg1[2048]={'\0'};
									char sendername[100]={'\0'};
									char sender[100]={'\0'};
									strncat(msg1,climsg,strlen(climsg)-1);
									strncpy(sendername,cli1->client_name,strlen(cli1->client_name)+1);
									strcat(msg,"FROM ");
									sprintf(sender,"%s :",sendername);
									strcat(msg,sender);
									strcat(msg,msg1);
									strcat(msg,"\n");
									msg[strlen(msg)]='\0';
									send(recipients_socket,msg,strlen(msg),0);//sending status message to client
									memset(msg,0,2048);
								}
								
								else
								{ 	
									//if the recipient is not registered in databse.
									printf("Unable to Locate Recipient (%s) in Database. Discarding MESG.\n",cliname); 
									char msg1[1000]="Unknown Recipient (";
									strcat(msg1,cliname);
									strcat(msg1,"). MESG Discarded.\n");
									send(cli1->socket_id,msg1,strlen(msg1),0);  //sending status message to client.
									memset(msg1,0,1000);
								}	
							}
							else
							{
								//condition if the Message is NULL
								printf("Message is NULL  Discarding MESG.\n"); 
								char msg1[1000]="Message is NULL-";
								strcat(msg1," MESG Discarded.\n");
								send(cli1->socket_id,msg1,strlen(msg1),0); 
								memset(msg1,0,1000);
							}
						}
							
						else 
						{	
							//if the client is attempting to send message to itself.
							printf("client (%d) :Unable to send message sender and receiver are same.\n",cli1->socket_id); 
							char msg1[1000]="Client can't send message to self -";
							strcat(msg1," MESG Discarded.\n");
							send(cli1->socket_id,msg1,strlen(msg1),0); 
							memset(msg1,0,1000);
						}
							
					
							
					}
					
						
					
					break;
						
						
			case 4:  
					// case for BCST query
					
					n=strlen(cli1->client_name);
					if(n==0) // checking if the client is registered or not to send broadcast message
					{	
						//for non registered user.
						printf("Unable to Locate Client (%d) in Database. Discarding BCST\n",cli1->socket_id);    
						send(cli1->socket_id,"Unregistered User. Use \"JOIN <username>\" to Register.\n",strlen("Unregistered User. Use \"JOIN <username>\" to Register.\n"), 0); //sending status message to client.
					}
					else 
					{
						//Sending the broadcast message sent by the registered client to all other registered clients except itself.
						int i=0;
						if(strlen(cli_bmsg)==0)
						{
								//condition if the Message is NULL
								printf("Message is NULL  Discarding BCST.\n"); 
								char msg1[1000]="Message is NULL(";
								strcat(msg1,"). BCST Discarded.\n");
								send(cli1->socket_id,msg1,strlen(msg1),0); 
								memset(msg1,0,1000);
								
								
						}
						else
						{
							
							while(i<max_clients)  
					
							{
								if(ClientsList[i]) //sending bcst message to all the registered users
								{
									char msg[1000]={'\0'};
									strcat(msg,"FROM ");
									strncat(msg,cli1->client_name,strlen(cli1->client_name)+1);
									strcat(msg,": ");
									strcat(msg,cli_bmsg);
									strcat(msg,"\n");
									if(ClientsList[i]->socket_id != cli1->socket_id)
										send(ClientsList[i]->socket_id,msg,strlen(msg),0); 
									memset(msg,0,1000);
								}
							
								i++;
							}
						}
					
					}
					
					break;
				
			case 5:
					//case for QUIT quert
					printf("Client (%d): QUIT\n",cli1->socket_id);
					n=strlen(cli1->client_name);
					//If the client is not registered in the database closing the connection upon QUIT request by the client.
					if(n==0)		
					{	
			
						printf("Unable to Locate Client (%d) in Database. Disconnecting User.\n",cli1->socket_id);
						printf("Client (%d): Disconnecting User.\n",cli1->socket_id);
						close(cli1->socket_id); //closing the client socket connection.
						cli1=NULL;// freeing up the struct variable memory.
						pthread_exit(NULL);//closing  the thread upon quit.
					}
					else
					{
					
						printf("Client (%d): Disconnecting User.\n",cli1->socket_id);
						//closing the client socket connection.
						int j=0;
						while(j<max_clients)   //Removing the client from the database when QUIT is requested
						{
							if(ClientsList[j])
							{
								if (strcmp(ClientsList[j]->client_name,cli1->client_name)==0)
								{	
							
									
									ClientsList[j]=NULL;
									break;
								}
							}
							j++;
						}
						close(cli1->socket_id);//closing the client socket connection.
						cli1=NULL;//freeing up the client variable
						clients_joined--; //decreasing the clients joined.
						pthread_exit(NULL);
						
					}
					break;
					
					
			default: 
					//default case for non query messages and for unregistered clients.
					printf("Client (%d): Unrecognizable Message. Discarding UNKNOWN Message\n",cli1->socket_id);  
					n=strlen(cli1->client_name);
					if(n==0)  
						send(cli1->socket_id,"Unregistered User. Use \"JOIN <username>\" to Register.\n",strlen("Unregistered User. Use \"JOIN <username>\" to Register.\n"), 0); 			
							
					else 
						send(cli1->socket_id,"Unknown Message. Discarding UNKNOWN Message.\n",strlen("Unknown Message. Discarding UNKNOWN Message.\n") ,0);			
					
		
		}	
	
	}

}      




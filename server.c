/*

AUTHOR: Abhijeet Rastogi (http://www.google.com/profiles/abhijeet.1989)

Editored by Xi Yu

*/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<signal.h>
#include<fcntl.h>
#include<pthread.h>
#include<sched.h>

#define CONNMAX 5
#define BYTES 1024

char *ROOT;
int listenfd, clients[CONNMAX];// listenfd is socket flag, cients[] accept flag
void error(char *);
void startServer(char *);
void *respond(int *);

#ifdef __APPLE__
#  define error printf
#endif

int main(int argc, char* argv[])
{
	struct sockaddr_in clientaddr;
	socklen_t addrlen;
	char c;    
	
	//Default Values PATH = ~/ and PORT=10000
	char PORT[6];
	ROOT = getenv("PWD");
	strcpy(PORT,"10000");

	int slot=0;

	//Parsing the command line arguments
	while ((c = getopt (argc, argv, "p:r:")) != -1)
		switch (c)
		{
			case 'r':
				ROOT = malloc(strlen(optarg));
				strcpy(ROOT,optarg);
				break;
			case 'p':
				strcpy(PORT,optarg);
				break;
			case '?':
				fprintf(stderr,"Wrong arguments given!!!\n");
				exit(1);
			default:
				exit(1);
		}
	
	printf("Server started at port no. %s%s%s with root directory as %s%s%s\n","\033[92m",PORT,"\033[0m","\033[92m",ROOT,"\033[0m");
	// Setting all elements to -1: signifies there is no client connected
	int i;
	for (i=0; i<CONNMAX; i++)
		clients[i]=-1;
	startServer(PORT);

	// ACCEPT connections
	while (1)
	{
		printf("waiting for connections...\n");
        addrlen = sizeof(clientaddr);
		clients[slot] = accept (listenfd, (struct sockaddr *) &clientaddr, &addrlen);

		if (clients[slot]<0)
			error ("accept() error");
		else
		{
			printf("connected to client!\n");
            pthread_t thread;
            pthread_attr_t tattr;
            int ret;
            int newprio = 20;
            struct sched_param param;
            ret = pthread_attr_init(&tattr);
            ret = pthread_attr_getschedparam (&tattr, &param);
            param.sched_priority = newprio;
            ret = pthread_attr_setschedparam (&tattr, &param);
            
            ret = pthread_create(&thread, &tattr, (void *)respond, &slot);
            pthread_join(thread, NULL);
            //int pthread_setschedparam(thread,SCHED_FIFO,struct sched_param *param);

            if (ret!=-1)
            {
                printf("responded in new thread\n\n");
                void pthread_exit(void *retval);
            }
            else
                printf("can't build new thread\n");
           
		}

		while (clients[slot]!=-1) slot = (slot+1)%CONNMAX;
	}

	return 0;
}

//start server
void startServer(char *port)
{
	struct addrinfo hints, *res, *p;

	// getaddrinfo for host
	memset (&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo( NULL, port, &hints, &res) != 0)
	{
		perror ("getaddrinfo() error");
		exit(1);
	}

	// socket and bind
	for (p = res; p!=NULL; p=p->ai_next)
	{
		listenfd = socket (p->ai_family, p->ai_socktype, 0);
		if (listenfd == -1) {perror("server:socket"); continue;}
		if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) {printf("bind success..\n"); break;}
	}
	if (p==NULL)
	{
		perror ("socket() or bind()");
		exit(1);
	}

	freeaddrinfo(res);

	// listen for incoming connections
	if ( listen (listenfd, 1000000) != 0 )
	{
		perror("listen() error");
		exit(1);
	}
    else printf("listen succes..\n");
}

//client connection
void *respond(int* num)
{
	int n = *num;
    
    char mesg[99999], *reqline[3], data_to_send[BYTES], path[99999];
	int rcvd, fd, bytes_read;

    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 

    printf("\n\n******************* new request ****************\n");
	memset( (void*)mesg, (int)'\0', 99999 );

	rcvd=recv(clients[n], mesg, 99999, 0);

	if (rcvd<0)    // receive error        
		{
            printf("recv() error\n");
            write(clients[n],"HTTP/1.0 200 error\n\n",20);    
	    }
    else if (rcvd==0)    // receive socket closed
		fprintf(stderr,"Client disconnected upexpectedly.\n");
	else    // message received
	{
		printf("%s", mesg);
		reqline[0] = strtok (mesg, " \t\n");
		if ( strncmp(reqline[0], "GET\0", 4)==0 )
		{
			reqline[1] = strtok (NULL, " \t");
			reqline[2] = strtok (NULL, " \t\n");
			if ( strncmp( reqline[2], "HTTP/1.0", 8)!=0 && strncmp( reqline[2], "HTTP/1.1", 8)!=0 )
			{
				write(clients[n], "HTTP/1.0 400 Bad Request\n", 25);
			}
			else
			{
				if ( strncmp(reqline[1], "/\0", 2)==0 )
					reqline[1] = "/index.html";        //Because if no file is specified, index.html will be opened by default (like it happens in APACHE...

				strcpy(path, ROOT);
				strcpy(&path[strlen(ROOT)], reqline[1]);
				printf("file: %s\n", path);

				if ( (fd=open(path, O_RDONLY))!=-1 )    //FILE FOUND
				{
					send(clients[n], "HTTP/1.0 200 OK\n\n", 17, 0);
					while ( (bytes_read=read(fd, data_to_send, BYTES))>0 )
						write (clients[n], data_to_send, bytes_read);
                    printf("send file successfull..\n");
				}
				else    write(clients[n], "HTTP/1.0 404 Not Found\n", 23); //FILE NOT FOUND
			}
		}
	}
    
	//Closing SOCKET
	shutdown (clients[n], SHUT_RDWR);         //blocking the socket connection..
	close(clients[n]);
	clients[n]=-1;    
    return NULL;
}

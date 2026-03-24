/* CHANGES FROM UNIX VERSION                                                   */
/*                                                                             */
/* 1.  Changed header files.                                                   */
/* 2.  Added WSAStartUP().                                                     */
/* 3.  Used Windows threads instead of POSIX.                                  */

#include <stdio.h>      /* for printf(), fprintf() */
#include <winsock.h>    /* for socket(),... */
#include <stdlib.h>     /* for exit() */

void DieWithError(char *errorMessage);  /* Error handling function */
void HandleTCPClient(int clntSocket);   /* TCP client handling function */
int CreateTCPServerSocket(unsigned short port); /* Create TCP server socket */
int AcceptTCPConnection(int servSock);  /* Accept TCP connection request */
void HandleTCPClient_version_2(int clntSocket, FILE* f); /* TCP echo server includes */
#include <dos.h>

void *ThreadMain(void *arg);            /* Main program of a thread */

/* Structure of arguments to pass to client thread */
struct ThreadArgs
{
    int clntSock;   
	int clientID;                   /* Socket descriptor for client */
};


DWORD  threadID;                 /* Thread ID from CreateThread() */
int main(int argc, char *argv[])
{
    int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
    unsigned short echoServPort;     /* Server port */
    //DWORD  threadID;                 /* Thread ID from CreateThread() */
    struct ThreadArgs *threadArgs;   /* Pointer to argument structure for thread */
    WSADATA wsaData;                 /* Structure for WinSock setup communication */
    int counter = 0 ;

    if (argc != 2)     /* Test for correct number of arguments */
    {
        fprintf(stderr,"Usage:  %s <SERVER PORT>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);  /* Local port */

    if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) /* Load Winsock 2.0 DLL */
    {
        fprintf(stderr, "WSAStartup() failed");
        exit(1);
    }

    servSock = CreateTCPServerSocket(echoServPort);

    for (;;) /* Run forever */
    {
		
       clntSock = AcceptTCPConnection(servSock); // ovo je blokirajuci poziv dakle samo ako neki klient zatrazi connnect nakon toga se ide dlaje 
       
       
        /* Create separate memory for client argument */
        threadArgs = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs)); // u ovom primeru to je balanno jer je mala memorija alo ima logike da ne punimo stack u slucaju da ima milion klienta puci ce stack 
        threadArgs -> clntSock = clntSock;
        threadArgs -> clientID = ++counter;

        if (CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) ThreadMain, threadArgs, 
              0, (LPDWORD) &threadID) == NULL)
            DieWithError("pthread_create() failed");
        printf("with thread %ld\n", threadID);
    }
    /* NOT REACHED */
}

void *ThreadMain(void *threadArgs)
{
	char filename[30];
	struct ThreadArgs *args = (struct ThreadArgs*)threadArgs;
	sprintf(filename, "sinus_samples_client_%d.txt", args->clientID);
	FILE *f = fopen(filename, "w");
    int clntSock;                   /* Socket descriptor for client connection */
    DWORD local_threadID=threadID;
    
    if (f == NULL) {
        printf("Error opening file\n");
        printf("try with creating new client");
        return 1; 
    }
    /* Guarantees that thread resources are deallocated upon return */
	fprintf(f, "Header : Client %d\n",args->clientID);  
    /* Extract socket file descriptor sfrom argument */
    clntSock = ((struct ThreadArgs *) threadArgs) -> clntSock;
    free(threadArgs);              /* Deallocate memory for argument */

    HandleTCPClient_version_2(clntSock, f); // nesto radi sa klijentom
    printf(" STOP thread %ld\n", local_threadID);

	fclose(f);
	if(args->clientID == 5) exit(1);
    return (NULL); 
}

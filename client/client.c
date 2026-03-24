

#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdint.h>
//#pragma comment(lib,"ws2_32.lib") //Winsock Library


#define RCVBUFSIZE 256   /* Size of receive buffer */

//   set  Project-> Project Options -> parameters -lws2_32
// ipconfig/all, tlenet IPaddress 17,13

uint32_t htonf(float f)
{
	uint32_t tmp ; 
	memcpy(&tmp, &f, sizeof(tmp));
	tmp = htonl(tmp);
	return tmp ;
}


void DieWithError(char *errorMessage);  /* Error handling function */

int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    unsigned short echoServPort;     /* Echo server port */
    char *servIP;                    /* Server IP address (dotted quad) */            
    int client_number ; 
    WSADATA wsaData;       
	uint32_t number_to_send;          /* Structure for WinSock setup communication */

    if ((argc < 3) || (argc > 4))  /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage: %s <Server IP> [<Echo Port>] [<client number(1-5)>]\n", argv[0]);
        exit(1);
    }
	
	else if ( argc == 3)
	{
		if( *argv[2]>'5'||  *argv[2]<'0') exit(1);
		client_number = atoi(argv[2]);
		echoServPort = 7 ;
	}
	else 
	{
		if( *argv[3]>'5'||  *argv[3]<'0') exit(1);
		echoServPort = atoi(argv[2]);
		client_number = atoi(argv[3]);
	}
    servIP = argv[1];             /* First arg: server IP address (dotted quad) */     /* Second arg: string to echo */

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) /* Load Winsock 2.0 DLL */
    {
        fprintf(stderr, "WSAStartup() failed");
        exit(1);
    }

    /* Create a reliable, stream socket using TCP */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));     /* Zero out structure */
    echoServAddr.sin_family      = AF_INET;             /* Internet address family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
    echoServAddr.sin_port        = htons(echoServPort); /* Server port */
    /* Establish the connection to the echo server */
    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("connect() failed");
        
    uint8_t Ts = 10 + client_number ;
    double f = 1.0 / (100 * Ts);
    
    int msec = 0; /* 10ms */
    uint16_t number_of_samples = 0 ;
    clock_t start = clock();
    while (number_of_samples < 100)
    {
	
		clock_t before = clock();
		do {
		clock_t difference = clock() - before;
		msec = (int)((double)difference * 1000.0 / CLOCKS_PER_SEC);
		
		} while ( msec < Ts );
		
		float  sample =  client_number * sinf(2 * M_PI * f * number_of_samples * Ts);
		number_to_send = htonf(sample);
		//if (send(sock, (uint32_t * )&number_to_send, sizeof(number_to_send), 0) != sizeof(number_to_send)) // ali posto je svjd koji pokazivac stavis bolje mozda char * jer njega interesuje samo pocetna adresa;
        if (send(sock, (char* )&number_to_send, sizeof(number_to_send), 0) != sizeof(number_to_send))
		DieWithError("send() sent a different number of bytes than expected");
        
        number_of_samples ++;
		
	}
	clock_t end = clock();
	int msec_global = (int)((double)(end - start) * 1000.0 / CLOCKS_PER_SEC);

	//echoString je poazivac na string koji se salje dakle imaj to u vidu kakvu poruka saljes 
    /* Send the string, including the null terminator, to the server */

	printf("Time taken %d seconds %d milliseconds\n",	msec_global/1000, msec_global%1000);
    printf("client %d has finished with work\n",client_number);    /* Print a final linefeed */

    closesocket(sock);
    WSACleanup();  /* Cleanup Winsock */


return 0;
    
}

void DieWithError(char *errorMessage)
{
    fprintf(stderr,"%s: %d\n", errorMessage, WSAGetLastError());
    exit(1);
}




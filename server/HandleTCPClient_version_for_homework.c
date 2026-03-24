/* CHANGES FROM UNIX VERSION                                                   */
/*                                                                             */
/* 1.  Changed header files.                                                   */
/* 2.  Used closesocket() instead of close().                                  */

#include <winsock.h>    /* for socket(),... */
#include <stdio.h>
#include <stdint.h>

#define RCVBUFSIZE 32   /* Size of receive buffer */
#define MAX_SAMPLES_SIZE 100

void DieWithError(char *errorMessage);  /* Error handling function */

float ntohf (uint32_t net_value)
{	
	uint32_t tmp = ntohl(net_value);
	float f; 
	memcpy(&f,&tmp, sizeof(f));
	return  f;
}

void HandleTCPClient_version_2(int clntSocket, FILE* f)
{      /* Buffer for echo string */
    int recvMsgSize;                    /* Size of received message */
	uint16_t samples_counter = 0 ;
	uint32_t net_value ; 
	uint8_t received = 0;
    /* Receive message from client */
    while(samples_counter < MAX_SAMPLES_SIZE)
    {
    	net_value = 0 ;
		received = 0 ;
		while(received < sizeof(net_value))
		{	
			if ((recvMsgSize= recv(clntSocket, (char*)&net_value + received, sizeof(net_value) - received, 0)) < 0)
		        DieWithError("recv() failed");
		    received += recvMsgSize ; 
		    
		}
		float number_for_print = ntohf(net_value);
		
		fprintf(f, "%f\n", number_for_print);  
		samples_counter ++;
		

	
	    /* Send received string and receive again until end of transmission */
	    
	    
	}

    closesocket(clntSocket);    /* Close client socket */
    
}

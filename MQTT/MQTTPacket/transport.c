

//#include <sys/types.h>

#if !defined(SOCKET_ERROR)
	/** error in socket operation */
	#define SOCKET_ERROR -1
#endif

#define INVALID_SOCKET SOCKET_ERROR

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


#include "lwip_comm.h" 
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "ethernetif.h" 
#include "lwip/timers.h"
#include "lwip/tcp_impl.h"
#include "lwip/ip_frag.h"
#include "lwip/tcpip.h" 
#include "malloc.h"
#include "delay.h"
#include "usart.h"  
#include <stdio.h>
#include "ucos_ii.h" 
#include "lwip/sockets.h"
#include "lwip/api.h"
#include "lwip/lwip_sys.h"
#include "lwip/igmp.h"
#include "lwip/inet.h"
#include "lwip/tcp.h"
#include "lwip/raw.h"
#include "lwip/udp.h"
#include "lwip/tcpip.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "transport.h"

/**
This simple low-level implementation assumes a single connection for a single thread. Thus, a static
variable is used for that connection.
On other scenarios, the user must solve this by taking into account that the current implementation of
MQTTPacket_read() has a function pointer for a function call to get the data to a buffer, but no provisions
to know the caller or other indicator (the socket id): int (*getfn)(unsigned char*, int)
*/

static int mysock = 0;


int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen)
{
	int rc = 0;
	rc = write(sock, buf, buflen);
	return rc;
}


int transport_getdata(unsigned char* buf, int count)
{
	int rc = recv(mysock, buf, count, 0);
	//printf("received %d bytes count %d\n", rc, (int)count);
	return rc;
}

int transport_getdatanb(void *sck, unsigned char* buf, int count)
{
	int sock = *((int *)sck); 	/* sck: pointer to whatever the system may use to identify the transport */
	/* this call will return after the timeout set on initialization if no bytes;
	   in your system you will use whatever you use to get whichever outstanding
	   bytes your socket equivalent has ready to be extracted right now, if any,
	   or return immediately */
	int rc = recv(sock, buf, count, 0);	
	if (rc == -1) {
		/* check error conditions from your system here, and return -1 */
		return 0;
	}
	return rc;
}


int transport_open(char* addr, int port)
{
	int* sock = &mysock;
	struct hostent *server;
	struct sockaddr_in serv_addr;

	int timeout = 1000;

	*sock = socket(AF_INET, SOCK_STREAM, 0);
	if(*sock < 0)
	{
		printf("[ERROR] Create socket failed\n");
	}		
	else
	{
	  printf("[SUCCESS] Create socket success\n");	
	}
	
	server = gethostbyname(addr); //
	if(server == NULL)
	{
		printf("[ERROR] Get host ip failed\n");
		return -1;
	}
	else
	{
		printf("[SUCCESS] Get host ip success\n");
	}
	
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);
	

	if(connect(*sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
	{
		printf("[ERROR] connect failed\n");
        return -1;
	}
    else
	{
		printf("[SUCCESS] Connect success\n");
	}
	setsockopt(mysock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout,sizeof(timeout));

    return mysock;
}

int transport_close(int sock)
{
	int rc;

	rc = shutdown(sock, SHUT_WR);
	rc = recv(sock, NULL, (size_t)0, 0);
	rc = close(sock);

	return rc;
}

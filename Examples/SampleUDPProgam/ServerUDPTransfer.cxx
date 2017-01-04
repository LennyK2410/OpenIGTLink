// Multicast Server
// written for LINUX
// Version 0.0.2
//
// Change: IP_MULTICAST_LOOP : Enable / Disable loopback for outgoing messages
//
// Compile : gcc -o client client.c
//
// This code has NOT been tested
//
#if defined(_WIN32) && !defined(__CYGWIN__)
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#pragma comment(lib "Ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>
#endif

#include <string.h>


#if defined(_WIN32) && !defined(__CYGWIN__)
#define WSA_VERSION MAKEWORD(1,1)
#define igtlCloseSocketMacro(sock) (closesocket(sock))
#else
#define igtlCloseSocketMacro(sock) (shutdown(sock, 2))
#endif


#define MAXBUFSIZE 65000 // Max UDP Packet size is 64 Kbyte

int main()
{
  int sock, status, socklen;
  char buffer[MAXBUFSIZE];
  struct sockaddr_in saddr;
  struct in_addr iaddr;
  unsigned char ttl = 255;
  unsigned char one = 1;
  
  // set content of struct saddr and imreq to zero
  memset(&saddr, 0, sizeof(struct sockaddr_in));
  memset(&iaddr, 0, sizeof(struct in_addr));
  
  // open a UDP socket
  sock = socket(PF_INET, SOCK_DGRAM, 0);
  if ( sock < 0 )
  {
    return 0;
  }
  
  saddr.sin_family = PF_INET;
  saddr.sin_port = htons(0); // Use the first free port
  saddr.sin_addr.s_addr = htonl(INADDR_ANY); // bind socket to any interface
  status = bind(sock, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
  
  if ( status < 0 )
  {
    return 0;
  }
  
  iaddr.s_addr = INADDR_ANY; // use DEFAULT interface

#if defined(_WIN32) && !defined(__CYGWIN__)
                             // Set the outgoing interface to DEFAULT
  setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, (const char*)&iaddr,
    sizeof(struct in_addr));

  // Set multicast packet TTL to 3; default TTL is 1
  setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (const char*)&ttl,
    sizeof(unsigned char));

  // send multicast traffic to myself too
  status = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP,
    (const char*)&one, sizeof(unsigned char));
#else
  // Set the outgoing interface to DEFAULT
  setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, &iaddr,
             sizeof(struct in_addr));
  
  // Set multicast packet TTL to 3; default TTL is 1
  setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl,
             sizeof(unsigned char));
  
  // send multicast traffic to myself too
  status = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP,
                      &one, sizeof(unsigned char));
#endif  
  // set destination multicast address
  saddr.sin_family = PF_INET;
  saddr.sin_addr.s_addr = inet_addr("226.0.0.1");
  saddr.sin_port = htons(4096);
  
  // put some data in buffer
  //strcpy(buffer, "Hello world\n");
  for (int i =0 ; i < MAXBUFSIZE; i++)
    buffer[i] = '1';
  
  socklen = sizeof(struct sockaddr_in);
  // receive packet from socket
  while(1)
  {
    status = sendto(sock, buffer, MAXBUFSIZE, 0,
                  (struct sockaddr *)&saddr, socklen);
    int i = strlen(buffer);
    int i2 = i;
  }
  
  // shutdown socket
  igtlCloseSocketMacro(sock);
  
  return 0;
}
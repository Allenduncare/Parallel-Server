/*
  From Stevens Unix Network Programming, vol 1.
  Minor modifications by John Sterling
*/

#include <stdio.h>       // perror, snprintf
#include <stdlib.h>      // exit
#include <unistd.h>      // close, write
#include <string.h>      // strlen
#include <strings.h>     // bzero
#include <sys/socket.h>  // socket, AF_INET, SOCK_STREAM,
                         // bind, listen, accept
#include <netinet/in.h>  // servaddr, INADDR_ANY, htons
#include <iostream>
#include <thread>
#include <mutex>
#include <ctime>
#include <cstdlib>
#include <vector>
#include "manager.h"
using namespace std;

#define MAXLINE   4096  // max text line length
#define BUFFSIZE  8192    // buffer size for reads and writes
#define  SA struct sockaddr
#define PORT_NUM 13002
#define LISTENQ   1024  // 2nd argument to listen()

int main(int argc, char **argv)
{
  int     listenfd, connfd;  // Unix file descriptors
  struct sockaddr_in  servaddr;          // Note C use of struct
  // 1. Create the socket
  if ((listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
    perror("Unable to create a socket");
    exit(1);
  }

  // 2. Set up the sockaddr_in

  // zero it.
  // bzero(&servaddr, sizeof(servaddr)); // Note bzero is "deprecated".  Sigh.
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family      = AF_INET; // Specify the family
  // use any network card present
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port        = htons(PORT_NUM); // daytime server
  // 3. "Bind" that address object to our listening file descriptor
  if (bind(listenfd, (SA *) &servaddr, sizeof(servaddr)) == -1) {
    perror("Unable to bind port");
  }

  // 4. Tell the system that we are going to use this sockect for
  //    listening and request a queue length
  if (listen(listenfd, LISTENQ) == -1) {
    perror("Unable to listen");
    exit(3);
  }
  RMmanager host(0, 13002,false);
  for ( ; ; ) {

        char buff[MAXLINE];
        // 5. Block until someone connects.
        //    We could provide a sockaddr if we wanted to know details of whom
        //    we are talking to.
        //    Last arg is where to put the size of the sockaddr if
        //    we asked for one
        fprintf(stderr, "Ready to connect.\n");
        if ((connfd = accept(listenfd, (SA *) NULL, NULL)) == -1) {
          perror("accept failed");
          exit(4);
        }
        fprintf(stderr, "Connected\n");
        // We had a connection.  Do whatever our task is.
        thread t1([&]{host.doStuff(connfd);});
        t1.detach();
        // 6. Close the connection with the current client and go back
        //    for another.
        close(connfd);
}
}

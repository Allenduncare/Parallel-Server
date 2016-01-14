#ifndef MANAGER_H
#define MANAGER_H

#include <stdio.h>       // perror, snprintf
#include <stdlib.h>      // exit
#include <unistd.h>      // close, write
#include <string.h>      // strlen
#include <strings.h>     // bzero
#include <time.h>        // time, ctime
#include <sys/socket.h>  // socket, AF_INET, SOCK_STREAM,
                         // bind, listen, accept
#include <netinet/in.h>  // servaddr, INADDR_ANY, htons
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <mutex>
#include <thread>
#include <arpa/inet.h>
#include "RM.h"
//header for managing all the RMs in the server.
#define MAXLINE   4096  // max text line length
#define BUFFSIZE  8192    // buffer size for reads and writes
#define  SA struct sockaddr
#define LISTENQ   1024  // 2nd argument to listen()
#define PORT_NUM  13002
#define FILE_END "EOF"
#define DELIMETER 178
#define TIME 30


class RMmanager{
private:
  std::vector<RM> RMVec;
  std::mutex userMut;
  std::mutex statusMut;
  std::mutex friendMut;
  int maxServerId;

  std::string getpassWord(const std::string& userName);
  bool checkPassword(const std::string& userName, const std::string& passWord);
  void addUser(const std::string& userName, const std::string& passWord);
  bool isUnique(const std::string& userName,const std::string& fileName);
  void follow(const std::string& follower, const std::string& followee);
  void unfollow(const std::string& follower,const std::string& followee);
  void updateStatus(const std::string& userName, std::string& message);
  bool changePassword(const std::string& username, const std::string& oldPassword, const std::string& newPassword);
  void displayuserNameMessages(const std::string& userName,int connfd);
  void copyFile(RM& rm);
  void writeData(const char *file, int sockfd);
  void getData(int connfd);
  void sendToRm(char* req);
public:
  RMmanager(int serverId, int port, bool status);
  void sync();
  void doStuff(int connfd);
};
void readFromClient(int connfd, char *buff, int size);
void writeToClient(int connfd, const char *buff, int size);

#endif

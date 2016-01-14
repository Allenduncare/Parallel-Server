#include "manager.h"
using namespace std;

bool RMmanager::checkPassword(const string& userName,const string& passWord)
{
    string userPass = getpassWord(userName);
    cout << userPass << endl;
    if (userPass == passWord)
      {
        return true;
      }
  return false;
}

string RMmanager::getpassWord(const string& userName)
{
userMut.lock();
  ifstream ifs("user.txt");
  if(!ifs)
    {
      cerr << "The file could not be read" << endl;
    }
    string fileUserName;
    string userPass;
  while (ifs >> fileUserName >> userPass)
    {
      cout << fileUserName << " " << userName << " " << userPass << endl;
      if (fileUserName == userName)
        {
          return userPass;
        }
    }
    userMut.unlock();
  return "";
}

void RMmanager::addUser(const string& userName, const string& passWord)
{
  userMut.lock();
  ofstream ofs("user.txt", ofstream::app);
  string str1 = userName + " "+ passWord +" "+'\r';
  ofs << str1;
  ofs.close();
  ofstream ofs1((userName+".txt"), ofstream::app);
  ofs1 << "0" << endl;
  ofs1.close();
  userMut.unlock();
}

bool RMmanager::isUnique(const string& userName,const string& fileName)
{
  userMut.lock();
  ifstream ifs(fileName);
  if(!ifs)
    {
      cerr << "The file could not be read" << endl;
    }
  string name;
  while(ifs>> name)
    {
      if (name == userName)
      {
        userMut.unlock();
        return false;
      }
    }
    userMut.unlock();
  return true;
}

void RMmanager::updateStatus(const string& userName, string& message)
{
  unique_lock<mutex> lock1(statusMut,defer_lock);
  statusMut.lock();
ofstream ofs("status.txt",ostream::app);
ofs << userName << " " << message << "\r\n";
ofs.close();
statusMut.unlock();
}

bool RMmanager::changePassword(const string& userName, const string& oldPassword, const string& newPassword)
{
  userMut.lock();
  if(getpassWord(userName)==oldPassword)
  {
    ifstream ifs("user.txt");
    ofstream ofs("newuser.txt");
    string password;
      string username;
    while(ifs >> username>> password )
  {
    if (username!=userName)
    {
      ofs << username << password << "\r\n";
    }
    else
    {
      ofs << userName << newPassword << "\r\n";
    }
  }
  ofs.close();
  remove("user.txt");
  rename("new.txt","user.txt");
  userMut.unlock();
  return true;
}
return false;
}

void RMmanager::displayuserNameMessages(const string& userName,int connfd )
{
  userMut.lock();
  statusMut.lock();
  ifstream ifs(userName+".txt");
  ifstream ifs1("status.txt");
  string user;
  string line;
  while( getline(ifs1,line))
  {
    istringstream iss(line);
    iss >> user;
    cout << line << endl;
    if(user == userName)
    {
      string message = "You said: ";
      string message2 = iss.eof() ? "" : iss.str().substr(iss.tellg());
      message += message2;
      message += "\r\n";
      cout << message << message2 << endl;
      char* reply= new char(message.length()-1);
      strcpy(reply,message.c_str());
      writeToClient(connfd,reply,strlen(reply));
    }
  }
  ifstream ifs2("status.txt");
  while (ifs >> user)
  {
    while(getline(ifs2,line));
    {
      istringstream iss(line);
      string name;
      iss >> name;
      if(name == user)
      {
        string message = user + " said: ";
        string message2 = iss.eof() ? "" : iss.str().substr(iss.tellg());
        message+=message2;
        message += "\r\n";
        cout << message << message2 << endl;
        char* reply= new char(message.length()-1);
        strcpy(reply,message.c_str());
        writeToClient(connfd,reply,strlen(reply));
      }
    }
    ifs.close();
  }
  userMut.unlock();
  statusMut.unlock();
}

void readFromClient(int connfd, char* buff, int size)
{
  if (read(connfd, buff, size) == -1)
    {
      perror("Read has failed");
    }
  for (int i = 0; i < size; ++i)
    {
      if (buff[i] == (char)DELIMETER)
      {
        buff[i] = '\0';
      }
    }
}


void writeToClient(int connfd, const char* buff, int size)
{
  int length = strlen(buff);
  if (length != write(connfd, buff, size))
    {
      perror("Write has failed");
    }
}

void RMmanager::follow(const string& follower, const string& followee)
{
 ofstream ofs(follower+".txt");
 ofs << followee << "\r\n";
 ofs.close();
}

void RMmanager::unfollow(const string& follower, const string& followee)
{
  friendMut.lock();
  ifstream ifs(follower+".txt");
  ofstream ofs("new.txt");
  string username;
  while(ifs >> username)
  {
  if (username != followee)
  {
    ofs << username << "\r\n";
  }}
  ofs.close();
  string fileName = follower+".txt";
  char* cstr= new char(fileName.length()-1);
  strcpy(cstr,fileName.c_str());
  remove(cstr);
  rename("new.txt",cstr);
  friendMut.unlock();
}


RMmanager::RMmanager(int serverId, int port, bool status){
  maxServerId = serverId;
  //Add self
  RMVec.push_back(RM(serverId, port, status));
  //Add more RMs here. # of RMs depend on expected failures
  RMVec.push_back(RM(100, 13003, true));
  RMVec.push_back(RM(100, 13004, true));
    //Find RM with highest serverId, and acknowledge that that RM is the primary one
  for (int i = 0; i < RMVec.size(); ++i){
    if (maxServerId < RMVec[i].getID())
      maxServerId = RMVec[i].getID();
  }

  //Synchronize RM with other RMs
  //Runs in separate thread because Synchronize gets called in SYNC_RATE intervals
  thread t([&] {
    sync();
  });
  t.detach();
}
void RMmanager::writeData(const char *file, int sockfd){
  ifstream ifs(file);
  char tmp;
  bool isEnd = false;
  while (ifs.get(tmp)) {
    string buff;
    buff.push_back(tmp);
    for (int i = 0; i < 4095; ++i) {
      if (ifs.get(tmp)) {
        buff.push_back(tmp);
      }
      else {
        writeToClient(sockfd, buff.c_str(), buff.length());
        isEnd = true;
        break;
      }
    }
    if (isEnd) break;
    writeToClient(sockfd, buff.c_str(), buff.length());
  }
  writeToClient(sockfd, FILE_END, 3);
  ifs.close();
}

void RMmanager::copyFile(RM& RM){
  unique_lock<mutex> lck1(userMut, defer_lock);
  unique_lock<mutex> lck2(statusMut, defer_lock);
  unique_lock<mutex> lck3(friendMut, defer_lock);
  lock(lck1, lck2, lck3);
  cout << "Writing to RM " << RM.getID() << endl;
  int sockfd;

  if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
    perror("Unable to create a socket");
    exit(1);
  }

  struct sockaddr_in servaddr;
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(RM.getPort());

  if((connect(sockfd, (SA *) &servaddr, sizeof(servaddr))) == -1) {
    perror("Unable to connect to server");
    return;
  }

  string message = "RMte~"; //~ means message was sent from RM, not front end
  writeToClient(sockfd, message.c_str(), message.length());
  writeData("user.txt", sockfd);
  writeData("status.txt", sockfd);
  ifstream ifs("user.txt");
  string userName;
  while(ifs>>userName)
  {
    string file = userName+".txt";
    char* cstr = new char(file.length()-1);
    strcpy(cstr,file.c_str());
   writeData(cstr,sockfd);
  }
  close(sockfd);
}

void RMmanager::getData(int connfd)
{
  unique_lock<mutex> lck1(userMut, defer_lock);
  unique_lock<mutex> lck2(statusMut, defer_lock);
  unique_lock<mutex> lck3(friendMut, defer_lock);
  lock(lck1, lck2, lck3);
  cout << "Receiving data\n";
  char buff[MAXLINE];
  ofstream ofs("user.txt", ofstream::trunc);
  readFromClient(connfd, buff, MAXLINE);
  while (strcmp(buff, FILE_END) != 0) {
    ofs << buff;
    readFromClient(connfd, buff, MAXLINE);
  }
  ofs.close();

  ofs.open("status.txt", ofstream::trunc);
  readFromClient(connfd, buff, MAXLINE);
  while (strcmp(buff, FILE_END) != 0) {
    ofs << buff;
    readFromClient(connfd, buff, MAXLINE);
  }
  ofs.close();
  string userName;
  ifstream ifs("user.txt",ofstream::trunc);
  while(ifs>>userName)
  {
    string file = userName+".txt";
    char* cstr = new char(file.length()-1);
    strcpy(cstr,file.c_str());
  ofs.open(cstr, ofstream::trunc);
  readFromClient(connfd, buff, MAXLINE);
  while (strcmp(buff, FILE_END) != 0) {
    ofs << buff;
    readFromClient(connfd, buff, MAXLINE);
  }
  ofs.close();
}
}
void RMmanager::sendToRm(char* req){
  if (RMVec[0].getID() != maxServerId) return; //Checks if primary rm or not.
  if (strlen(req) > 0) {
    strcat(req, "~"); //messages ending with ~ are rm replicating messages
    for (int i = 1; i < RMVec.size(); ++i) {
      if (RMVec[i].getStatus()) {
        cout << "Sending request to RM " << RMVec[i].getID() << endl;
        int sockfd;

        if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
          perror("Unable to create a socket");
          exit(1);
        }

        struct sockaddr_in servaddr;
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        servaddr.sin_port = htons(RMVec[i].getPort());

        if((connect(sockfd, (SA *) &servaddr, sizeof(servaddr))) == -1) {
          perror("Unable to connect to server");
          return;
        }
        writeToClient(sockfd, req, strlen(req));
        close(sockfd);
      }
    }
  }
}

void RMmanager::sync(){
  while (true) {
    //Wait for other RMs to sync this one before this one syncs
    this_thread::sleep_for(chrono::seconds(TIME));
    cout << "Synchronizing\n";
    maxServerId = RMVec[0].getID();
    char buf[MAXLINE];
    for (int i = 1; i < RMVec.size(); ++i) {
      int sockfd;

      if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        perror("Unable to create a socket");
        exit(1);
      }

      struct sockaddr_in servaddr;
      memset(&servaddr, 0, sizeof(servaddr));
      servaddr.sin_family = AF_INET;
      servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
      servaddr.sin_port = htons(RMVec[i].getPort());

      //Check if RM is running by trying to connect
      if((connect(sockfd, (SA *) &servaddr, sizeof(servaddr))) == -1) {
        perror("Unable to connect to server");
        RMVec[i].changeStatus(false);
      }
      else {
        //If able to connect, update maxServerId
        maxServerId = (RMVec[i].getID() > maxServerId) ? RMVec[i].getID() : maxServerId;
        //If the RMs status was false, and it is now running
        //Then it just joined, or started to run after being down
        //So its files need to be updated
        if (!RMVec[i].getStatus()) {
          RMVec[i].changeStatus(true);
          copyFile(RMVec[i]);
        }
        close(sockfd);
      }
    }
    RMVec[0].changeStatus(true);
  }
}



//Handles requests from clients
void RMmanager::doStuff(int connfd)
{
  cout << "Connected\n";
  char req[MAXLINE];
  readFromClient(connfd, req, MAXLINE);

  //If current RM still has false as status, it has not synced, and should not do anyhting
  if (!RMVec[0].getStatus()){
    close(connfd);
    cout << "Connection closed\n\n";
    return;
  }
  //If current RM is not primary RM, and it receives request from froont end, do nothing
  //A request ending with "~" means it was from another RM
  if (RMVec[0].getID() != maxServerId) {
    if (req[strlen(req) - 1] != '~'){
      close(connfd);
      return;
    }
    int length = strlen(req) - 1;
    req[length] = '\0';
  }

  istringstream iss(req);
  string request;
  string userName;
  string passWord;
  string newPassword;
  string Text;
  iss >> request >> userName;
  cout << request << " "<< userName << endl;

  if(request == "changePassword")
    {
    cout << "0" << endl;
    iss >> passWord>> newPassword;
    if(changePassword(userName,passWord,newPassword))
    {
      char reply[] = "Password changed";
       writeToClient(connfd,reply,strlen(reply));}
    else
    {
      char reply[] = "Incorrect password";
      writeToClient(connfd,reply,strlen(reply));}
    }

  if(request == "checkPassword")
    {
    cout << "1" << endl;
    iss >> passWord;
    if(checkPassword(userName,passWord))
    {
      char reply[] = "Correct password";
       writeToClient(connfd,reply,strlen(reply));}
    else
    {
      char reply[] = "Incorrect password";
      writeToClient(connfd,reply,strlen(reply));}
    }

if(request == "addUser")
    {
    cout << "2" << endl;
    if (isUnique(userName,"user.txt"))
    {
      cout << "3" << endl;
      iss >> passWord;
      addUser(userName,passWord);
      cout <<userName << " " << passWord << endl;
      char reply[] = "Success";
      writeToClient(connfd,reply,strlen(reply));
    }
    else
    {
      cout << "4" << endl;
      char reply[] = "Username is taken";
      writeToClient(connfd,reply,strlen(reply));
    }
  }
if(request  == "follow")
    {
    cout << "5" << endl;
    string followee;
    iss >> followee;
    if(!isUnique(followee,"user.txt"))
      {
        if(isUnique(followee,(userName+".txt")))
        {
          follow(userName,followee);
          string reply = "Now following "+followee;
          char* cstr = new char(reply.length()-1);
          strcpy(cstr,reply.c_str());
          writeToClient(connfd,cstr,strlen(cstr));
        }
    else
      {
        cout << "6" << endl;
        string reply = "You are already following "+followee;
        char* cstr = new char(reply.length()-1);
        strcpy(cstr,reply.c_str());
        writeToClient(connfd,cstr,strlen(cstr));
      }
    }
  else
    {
      string reply = followee + " does not exist!";
      char* cstr = new char(reply.length()-1);
      strcpy(cstr,reply.c_str());
      writeToClient(connfd,cstr,strlen(cstr));
    }
  }
if(request == "unfollow")
  {
    cout << "7" << endl;
    string followee;
    iss >> followee;
    if(isUnique(followee,(userName+".txt")))
    {
      string reply = "You are not following "+followee;
      char* cstr = new char(reply.length()-1);
      strcpy(cstr,reply.c_str());
      writeToClient(connfd,cstr,strlen(cstr));
    }
    else
    {
    cout << "8" << endl;
    unfollow(userName,followee);
    string reply = "You are no longer following "+followee;
    char* cstr = new char(reply.length()-1);
    strcpy(cstr,reply.c_str());
    writeToClient(connfd,cstr,strlen(cstr));
    }
}

if(request == "updateStatus")
{
  cout << "9" << endl;
  string message = iss.eof() ? "" : iss.str().substr(iss.tellg());
  updateStatus(userName,message);
  char reply[] = "Status has been updated!";
  writeToClient(connfd,reply,strlen(reply));
}

if(request == "displayuserNameMessages")
{
  cout << "10" << endl;
  displayuserNameMessages(userName,connfd);
}



}

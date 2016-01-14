#ifndef RM_H
#define RM_H
#include <string>

class RM{
private:
  int serverId;
  int port;
  bool status;

public:
  RM(int serverId, int port, bool status);
  int getPort() const;
  bool getStatus() const;
  int getID() const;
  bool changeStatus(bool newStatus);
};

#endif

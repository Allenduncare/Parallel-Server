#include "RM.h"
using namespace std;

RM::RM(int serverId, int port, bool status)
  :serverId(serverId),port(port), status(status) {}

int RM::getPort() const{
  return port;
}

bool RM::getStatus() const{
  return status;
}
int RM::getID() const{
  return serverId;
}
bool RM::changeStatus(bool newStatus){
  status = newStatus;
}

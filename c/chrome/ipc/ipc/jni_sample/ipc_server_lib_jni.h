#ifndef __IPC_SERVER_LIB_JNI_H__
#define __IPC_SERVER_LIB_JNI_H__

#include "ipc_server_lib.h"

class IPC_SERVER_JNI: public IPC_SERVER {
public:
  IPC_SERVER_JNI(): IPC_SERVER() {
  }
  IPC_SERVER_JNI(std::string name): IPC_SERVER(name){
  }

private:
  bool OnMessageReceived(const IPC::Message& message);
  void OnHelloAsync(std::string str);
};

class IPC_SERVER_JNI_SYNC: public IPC_SERVER_SYNC {
public:
  IPC_SERVER_JNI_SYNC(): IPC_SERVER_SYNC() {
  }
  IPC_SERVER_JNI_SYNC(std::string name): IPC_SERVER_SYNC(name){
  }

private:
  bool OnMessageReceived(const IPC::Message& message);
  void OnHello(std::string str, std::string *value);
};

#endif

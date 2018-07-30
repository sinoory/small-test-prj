#ifndef __IPC_SERVER_TEST_H__
#define __IPC_SERVER_TEST_H__
#include "ipc_server_lib.h"

class IPC_SERVER_C : public IPC_SERVER{
public:
  IPC_SERVER_C(): IPC_SERVER() {
  }
  IPC_SERVER_C(std::string name): IPC_SERVER(name) {
  }

private:
  bool OnMessageReceived(const IPC::Message& message);
  void OnHelloAsync(std::string str);
};

class IPC_SERVER_SYNC_TEST: public IPC_SERVER_SYNC {
public:
  IPC_SERVER_SYNC_TEST(): IPC_SERVER_SYNC() {}
  IPC_SERVER_SYNC_TEST(std::string name): IPC_SERVER_SYNC(name) {}

private:
  bool OnMessageReceived(const IPC::Message& message);
  void OnHello(std::string str, std::string *value);
};

#endif

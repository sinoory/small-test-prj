#include "ipc_server_test.h"
#include <android/log.h>
#include "ipc_msg_lib.h"
#include "base/logging.h"

#define LOGD(...) __android_log_print(ANDROID_LOG_INFO, "IPC_TEST", __VA_ARGS__)


bool IPC_SERVER_C::OnMessageReceived(const IPC::Message& message) {
  LOG(INFO)<<__func__;
  LOGD("file %s func %s enter", __FILE__, __func__);
  IPC_BEGIN_MESSAGE_MAP(IPC_SERVER_C, message)
  IPC_MESSAGE_HANDLER(IPC_MSG_Test_Hello, OnHelloAsync)
  IPC_END_MESSAGE_MAP()
  return true;
}

void IPC_SERVER_C::OnHelloAsync(std::string str) {
  LOG(INFO)<<__func__;
  LOGD("func %s receive message %s", __func__, str.c_str());
  Send(new IPC_MSG_Test_Hello(MSG_ROUTING_NONE, std::string("Server_C")));
}

bool IPC_SERVER_SYNC_TEST::OnMessageReceived(const IPC::Message& message) {
  LOGD("file %s func %s enter", __FILE__, __func__);
  LOG(INFO)<<__func__;
  IPC_BEGIN_MESSAGE_MAP(IPC_SERVER_SYNC_TEST, message)
  IPC_MESSAGE_HANDLER(IPC_SYNC_Test_Hello, OnHello)
  IPC_END_MESSAGE_MAP()
  return true;
}

int num = 0;

void IPC_SERVER_SYNC_TEST::OnHello(std::string str, std::string *value) {
  LOG(INFO)<<__func__;
  LOGD("func %s receive message %s", __func__, str.c_str());
  *value = "Jian_C";
  if(num == 0) {
    num = 1;
    std::string value;
    Send(new IPC_SYNC_Test_Hello(MSG_ROUTING_NONE, std::string("Server_sync_c"), &value));
    LOGD("func %s server_c receive %s", __func__, value.c_str());
  }
}

int main() {
  std::string pipeName = "/data/local/tmp/IPC_ASYNC";
  IPC_SERVER_C *tmp_server = new IPC_SERVER_C();
  tmp_server->init(pipeName);

  IPC_SERVER_SYNC_TEST *sync_server = new IPC_SERVER_SYNC_TEST();
  sync_server->init("/data/local/tmp/IPC_SYNC");

  LOGD("func %s waiting message pipename %s...", __func__, tmp_server->getName().c_str());
  while(1);
}

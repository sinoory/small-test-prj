#include "ipc_client_lib.h"
#include <unistd.h>
#include <fcntl.h>
#include <android/log.h>
#include "base/logging.h"

#define LOGD(...) __android_log_print(ANDROID_LOG_INFO, "IPC_TEST", __VA_ARGS__)

class IPC_CLIENT_TEST : public IPC_CLIENT {
public:
  IPC_CLIENT_TEST(): IPC_CLIENT() {}
  IPC_CLIENT_TEST(std::string pipeName): IPC_CLIENT(pipeName) {}

private:
  bool OnMessageReceived(const IPC::Message& message) override {
    LOGD("file %s func %s enter", __FILE__, __func__);
    LOG(INFO)<<__func__;
    IPC_BEGIN_MESSAGE_MAP(IPC_CLIENT_TEST, message)
    IPC_MESSAGE_HANDLER(IPC_MSG_Test_intro, OnIntroAsync)
    IPC_MESSAGE_HANDLER(IPC_MSG_Test_Hello, OnHelloAsync)
    IPC_END_MESSAGE_MAP()
    return true;

  }

  void OnIntroAsync(int fd, std::string str) {
    LOG(INFO)<<__func__;
    LOGD("file %s func %s fd %d value %s", __FILE__, __func__, fd, str.c_str());
  }
  void OnHelloAsync(std::string str) {
    LOG(INFO)<<__func__;
    LOGD("file %s func %s value %s", __FILE__, __func__, str.c_str());
  }
};

class IPC_CLIENT_SYNC_TEST : public IPC_CLIENT_SYNC {
public:
  IPC_CLIENT_SYNC_TEST(): IPC_CLIENT_SYNC() {}
  IPC_CLIENT_SYNC_TEST(std::string pipeName): IPC_CLIENT_SYNC(pipeName) {}

  bool OnMessageReceived(const IPC::Message& message) override {
    LOG(INFO)<<__func__;
    LOGD("file %s func %s enter", __FILE__, __func__);
    IPC_BEGIN_MESSAGE_MAP(IPC_CLIENT_SYNC_TEST, message)
    IPC_MESSAGE_HANDLER(IPC_SYNC_Test_Hello, OnHello)
    IPC_END_MESSAGE_MAP()
    return true;
  }

  void OnIntro(int fd, std::string str, std::string *value) {
    LOG(INFO)<<__func__;
    LOGD("file %s func %s fd %d value %s", __FILE__, __func__, fd, str.c_str());
    *value="hello world";
  }
  void OnHello(std::string str, std::string *value) {
    LOGD("file %s func %s value %s", __FILE__, __func__, str.c_str());
    *value= "Jian_hello";
  }
};

int main(int argc, char **argv) {

    LOG(INFO)<<__func__;
  IPC_CLIENT_TEST client;
  client.init("/data/local/tmp/IPC_ASYNC");
  LOGD("client main pid %ld", pthread_self());
  bool ret = false;
  ret = client.Send(new IPC_MSG_Test_Hello(MSG_ROUTING_NONE, std::string("hello gay")));
  if(!ret)
    LOGD("client send message fail");
  IPC_CLIENT_SYNC *client_sync = new IPC_CLIENT_SYNC_TEST();
  client_sync->init("/data/local/tmp/IPC_SYNC");
  std::string value;
  ret = client_sync->Send(new IPC_SYNC_Test_Hello(MSG_ROUTING_NONE, std::string("Jian_Sync_Client"), &value));
  LOGD("func %s receive sync message %s", __func__, value.c_str());
    LOG(INFO)<<__func__;
  while(1) {
      sleep(5);
  }
}


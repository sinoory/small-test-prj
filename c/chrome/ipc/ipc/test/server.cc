#include "sraf/ipc/lib/lib_ipc.h"

#define IPC_MESSAGE_IMPL 1
#include "ipc_test_messages.h"

#if defined(OS_ANDROID)
#define SRAF_NAMED_UNIX_SOCK_TEST_PATH "/data/local/tmp/sraf_named_domain_socket_test"
#else
#define SRAF_NAMED_UNIX_SOCK_TEST_PATH "/tmp/sraf_named_domain_socket_test"
#endif

#include "base/at_exit.h"

class IPCChannelPosixTestListener;

const std::string getTestConnectionSocketName() {
  return SRAF_NAMED_UNIX_SOCK_TEST_PATH;
}

//FIXME: LOG(INFO) in Listener will crash ?
class IPCChannelPosixTestListener : public SrafIpcListener {
 public:
  enum STATUS {
    DISCONNECTED,
    MESSAGE_RECEIVED,
    CHANNEL_ERROR,
    CONNECTED,
    DENIED,
    LISTEN_ERROR
  };

  IPCChannelPosixTestListener(bool quit_only_on_message)
      : status_(DISCONNECTED){
  }

  ~IPCChannelPosixTestListener() override {
  }


  bool OnMessageReceived(const IPC::Message& message) override {
    status_ = MESSAGE_RECEIVED;
    IPC_BEGIN_MESSAGE_MAP(IPCChannelPosixTestListener, message)
      IPC_MESSAGE_HANDLER(Msg_C_3_3, On_3_3)
      IPC_MESSAGE_HANDLER(Msg_R_3_3, On_3_3)
    IPC_END_MESSAGE_MAP()
    return true;
  }

  void OnChannelConnected(int32_t peer_pid) override {
    status_ = CONNECTED;
  }

  void On_3_3(int in1,const std::string& in2,bool in3,
              std::string* out1,int* out2,bool* out3) {
    LOG(INFO)<<__func__;
    *out1 = "this_reply_from_server";
    *out2 = 33;
    *out3 = false;
  }


  STATUS status() { return status_; }
 private:
  // The current status of the listener.
  STATUS status_;
};

//FIXME: rm base::AtExitManager ?
int main(){
  base::AtExitManager shadow;
  LOG(INFO)<<__func__;
  IPCChannelPosixTestListener slistener(false);
  SrafIpcThread serverIpcThread;
  serverIpcThread.initChannal(SrafIpcThread::C_SERVER,getTestConnectionSocketName(),&slistener);
  LOG(INFO)<<__func__;
  sleep(1);//avoid init channal at the same time

  bool bool1 = true;
  int int1 = 0;
  std::string string1;
  LOG(INFO)<<__func__;
  serverIpcThread.Send(new Msg_C_3_3(3, "this_msg_from_server", true, &string1, &int1, &bool1));//send sync msg
  LOG(INFO)<<__func__<<" Server : client sync reply is "<<string1;

  sleep(5);
}


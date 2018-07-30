#include "sraf/ipc/lib/lib_ipc.h"

#define IPC_MESSAGE_IMPL 1
#include "ipc_test_messages.h"

#if defined(OS_ANDROID)
#define SRAF_NAMED_UNIX_SOCK_TEST_PATH "/data/local/tmp/sraf_named_domain_socket_test"
#else
#define SRAF_NAMED_UNIX_SOCK_TEST_PATH "/tmp/sraf_named_domain_socket_test"
#endif

#include "base/at_exit.h"

#if defined(OS_ANDROID)
#define SRAF_NAMED_UNIX_SOCK_TEST_PATH "/data/local/tmp/sraf_named_domain_socket_test"
#else
#define SRAF_NAMED_UNIX_SOCK_TEST_PATH "/tmp/sraf_named_domain_socket_test"
#endif


const std::string getTestConnectionSocketName() {
  return SRAF_NAMED_UNIX_SOCK_TEST_PATH;
}


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
    //LOG(INFO)<<__func__<<" in";
    status_ = MESSAGE_RECEIVED;
    IPC_BEGIN_MESSAGE_MAP(IPCChannelPosixTestListener, message)
      IPC_MESSAGE_HANDLER(Msg_C_3_3, On_3_3)
      IPC_MESSAGE_HANDLER(Msg_R_3_3, On_3_3)
    IPC_END_MESSAGE_MAP()
    //LOG(INFO)<<__func__<<" out";
    return true;
  }

  void OnChannelConnected(int32_t peer_pid) override {
    //LOG(INFO)<<__func__<<" peer="<<peer_pid;
    status_ = CONNECTED;
  }

  void OnChannelError() override {
    status_ = CHANNEL_ERROR;
  }

  void OnChannelDenied() override {
    status_ = DENIED;
  }

  void OnChannelListenError() override {
    status_ = LISTEN_ERROR;
  }

  void On_3_3(int in1,const std::string& in2,bool in3,
              std::string* out1,int* out2,bool* out3) {
    //LOG(INFO)<<__func__<<" in str="<<in2;
    *out1 = "3_3";
    *out2 = 33;
    *out3 = false;
  }


  STATUS status() { return status_; }
 private:
  // The current status of the listener.
  STATUS status_;
};

int main(){
  base::AtExitManager shadow;
  LOG(INFO)<<__func__;
  IPCChannelPosixTestListener slistener(false);
  SrafIpcThread serverIpcThread;
  serverIpcThread.initChannal(SrafIpcThread::C_SERVER,getTestConnectionSocketName(),&slistener);
  sleep(1);//avoid init channal at the same time
  SrafIpcThread clientIpcThread;
  IPCChannelPosixTestListener clistener(false);
  clientIpcThread.initChannal(SrafIpcThread::C_CLIENT,getTestConnectionSocketName(),&clistener);

#if 1
  sleep(1);//wait the channel connect
  clientIpcThread.Send(new Msg_C_0()); //send async msg
  sleep(1);//wait message received

  bool bool1 = true;
  int int1 = 0;
  std::string string1;
  clientIpcThread.Send(new Msg_C_3_3(3, "3_3", true, &string1, &int1, &bool1));//send sync msg
  LOG(INFO)<<__func__<<" client rcv msg "<<string1;

 
  string1="";
  serverIpcThread.Send(new Msg_C_3_3(3, "3_3", true, &string1, &int1, &bool1));//send sync msg
  LOG(INFO)<<__func__<<" server rcv msg "<<string1;
#endif
}


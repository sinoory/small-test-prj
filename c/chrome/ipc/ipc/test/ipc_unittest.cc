#include "ipc/ipc_channel_posix.h"
#include "sraf/ipc/lib/lib_ipc.h"

#define IPC_MESSAGE_IMPL 1
#include "ipc_test_messages.h"

#include <fcntl.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "base/debug/stack_trace.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/location.h"
#include "base/memory/scoped_ptr.h"
#include "base/path_service.h"
#include "base/posix/eintr_wrapper.h"
#include "base/process/process.h"
#include "base/single_thread_task_runner.h"
#include "base/test/multiprocess_test.h"
#include "base/test/test_timeouts.h"
#include "ipc/ipc_listener.h"
#include "ipc/unix_domain_socket_util.h"
#include "testing/multiprocess_func_list.h"



namespace {



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
    LOG(INFO)<<__func__<<" in";
    status_ = MESSAGE_RECEIVED;
    IPC_BEGIN_MESSAGE_MAP(IPCChannelPosixTestListener, message)
      IPC_MESSAGE_HANDLER(Msg_C_3_3, On_3_3)
      IPC_MESSAGE_HANDLER(Msg_R_3_3, On_3_3)
    IPC_END_MESSAGE_MAP()
    LOG(INFO)<<__func__<<" out";
    return true;
  }

  void OnChannelConnected(int32_t peer_pid) override {
    LOG(INFO)<<__func__<<" peer="<<peer_pid;
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
    LOG(INFO)<<__func__<<" in str="<<in2;
    DCHECK_EQ(3, in1);
    DCHECK_EQ("3_3", in2);
    DCHECK(in3);
    *out1 = "3_3";
    *out2 = 33;
    *out3 = false;
  }


  STATUS status() { return status_; }
 private:
  // The current status of the listener.
  STATUS status_;
};


class IPCChannelPosixTest : public base::MultiProcessTest {
 public:
  static void SetUpSocket(IPC::ChannelHandle *handle,
                          IPC::Channel::Mode mode);
  static void SpinRunLoop(base::TimeDelta delay);
  static const std::string GetChannelDirName();

 protected:
  void SetUp() override;
  void TearDown() override;

 private:
  scoped_ptr<base::MessageLoopForIO> message_loop_;
};

//TODO: test channal memory
//TODO: fix init channal at the same time

const std::string IPCChannelPosixTest::GetChannelDirName() {
  base::FilePath tmp_dir;
  PathService::Get(base::DIR_TEMP, &tmp_dir);
  return tmp_dir.value();
}

void IPCChannelPosixTest::SetUp() {
  LOG(INFO)<<__func__;
  MultiProcessTest::SetUp();
  // Construct a fresh IO Message loop for the duration of each test.
  message_loop_.reset(new base::MessageLoopForIO());
}

void IPCChannelPosixTest::TearDown() {
  message_loop_.reset(NULL);
  MultiProcessTest::TearDown();
}
#if 1
TEST(CTest , OneProcessIpcTest) {
  LOG(INFO)<<__func__;
  IPCChannelPosixTestListener slistener(false);
  SrafIpcThread serverIpcThread;
  serverIpcThread.initChannal(SrafIpcThread::C_SERVER,getTestConnectionSocketName(),&slistener);
  sleep(1);//avoid init channal at the same time
  SrafIpcThread clientIpcThread;
  IPCChannelPosixTestListener clistener(false);
  clientIpcThread.initChannal(SrafIpcThread::C_CLIENT,getTestConnectionSocketName(),&clistener);

  sleep(1);//wait the channel connect
  clientIpcThread.Send(new Msg_C_0()); //send async msg
  sleep(1);//wait message received
  EXPECT_EQ(IPCChannelPosixTestListener::MESSAGE_RECEIVED, slistener.status());

  bool bool1 = true;
  int int1 = 0;
  std::string string1;
  clientIpcThread.Send(new Msg_C_3_3(3, "3_3", true, &string1, &int1, &bool1));//send sync msg
  EXPECT_EQ("3_3", string1);

 
  string1="";
  serverIpcThread.Send(new Msg_C_3_3(3, "3_3", true, &string1, &int1, &bool1));//send sync msg
  EXPECT_EQ("3_3", string1);

}
#endif

TEST(CTest , OneProcessMultiConnectIpcTest) {
  int cnt=1;
  while(cnt>0){
    LOG(INFO)<<__func__;
    IPCChannelPosixTestListener slistener(false);
    SrafIpcThread serverIpcThread;
    serverIpcThread.initChannal(SrafIpcThread::C_SERVER,getTestConnectionSocketName(),&slistener);

    SrafIpcThread clientIpcThread;
    IPCChannelPosixTestListener clistener(false);
    clientIpcThread.initChannal(SrafIpcThread::C_CLIENT,getTestConnectionSocketName(),&clistener);

    sleep(1);
    bool bool1 = true;
    int int1 = 0;
    std::string string1;
    clientIpcThread.Send(new Msg_C_3_3(3, "3_3", true, &string1, &int1, &bool1));//send sync msg
    EXPECT_EQ("3_3", string1);

    cnt-=1;
  }
}

#if 1
TEST(CTest , OneProcessWithMainLoopTest) {
  base::MessageLoopForIO mainio;
  SrafIpcThread serverIpcThread;
  SrafIpcThread clientIpcThread;

  IPCChannelPosixTestListener slistener(false);
  serverIpcThread.initChannal(SrafIpcThread::C_SERVER,getTestConnectionSocketName(),&slistener);
  sleep(1);//sleep to avoid create the same socket at the same time
  IPCChannelPosixTestListener clistener(false);
  clientIpcThread.initChannal(SrafIpcThread::C_CLIENT,getTestConnectionSocketName(),&clistener);
#if 1
  sleep(1);//wait the channel connect
  clientIpcThread.Send(new Msg_C_0()); //send async msg
  sleep(1);//wait message received
  EXPECT_EQ(IPCChannelPosixTestListener::MESSAGE_RECEIVED, slistener.status());
#endif

}
#endif

#if 1
TEST_F(IPCChannelPosixTest, TwoProcessIpcTest) {
  IPCChannelPosixTestListener slistener(false);
  SrafIpcThread serverIpcThread;
  serverIpcThread.initChannal(SrafIpcThread::C_SERVER,getTestConnectionSocketName(),&slistener);
  base::Process process = SpawnChild("IPCChannelPosixTestClientConnectionProc");

  ASSERT_TRUE(process.IsValid());

  sleep(1);//wait child process init
  serverIpcThread.Send(new Msg_C_0()); //send async msg
  sleep(1);
  int exit_code = 0;
  EXPECT_TRUE(process.WaitForExit(&exit_code));
  EXPECT_EQ(0, exit_code);
  //unlink(chan_handle.name.c_str());
  //ASSERT_TRUE(false);
}
#endif
// A long running process that connects to us
MULTIPROCESS_TEST_MAIN(IPCChannelPosixTestClientConnectionProc) {
  SrafIpcThread clientIpcThread;
  IPCChannelPosixTestListener clistener(false);
  clientIpcThread.initChannal(SrafIpcThread::C_CLIENT,getTestConnectionSocketName(),&clistener);
  LOG(INFO)<<__func__<<" cur loop = "<<base::MessageLoop::current();
  sleep(3);
  EXPECT_EQ(IPCChannelPosixTestListener::MESSAGE_RECEIVED, clistener.status());
  return 0;
}

}


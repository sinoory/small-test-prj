
#include "lib_ipc.h"
#include "ipc/ipc_sync_channel.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/time/time.h"

#include <fcntl.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

bool SrafIpcListener::OnMessageReceived(const IPC::Message& message){
  return false;
}
bool SrafIpcListener::Send(IPC::Message* message) {
  if (_sender) {
    _sender->Send(message);
    return true;
  }
  return false;
}
void SrafIpcListener::setSender(IPC::Sender* sender){
  _sender = sender;
}

SrafIpcThread::~SrafIpcThread(){
}

SrafIpcThread::SrafIpcThread(std::string threadname):_thread_name(threadname){
  if (false && base::MessageLoopForIO::IsCurrent()){
    _io_msg_loop = base::MessageLoopForIO::current();
  }else{
    _thread.reset(new base::Thread(threadname+"_io"));
    base::Thread::Options options;
    options.message_loop_type = base::MessageLoop::TYPE_IO;
    _thread->StartWithOptions(options);
    _thread->WaitUntilThreadStarted();
    _io_msg_loop = _thread->message_loop();
    _listen_thread.reset(new base::Thread(threadname+"_ls"));
    _listen_thread->StartWithOptions(options);
    _listen_thread->WaitUntilThreadStarted();
  }
}

bool SrafIpcThread::Send(IPC::Message *message){
  if(_sync_channel)
    return _sync_channel->Send(message);
  return false;
}

void SrafIpcThread::initChannal(EChannalType t,std::string channal_name,SrafIpcListener* listener){
  _channal_type = t;
  _channal_name = channal_name;
  _listener = listener;
  //ipc_task_runner_ and listern need use different task_runner, otherwise deadlock
  //see ChannelProxy::Context::Context
  int delayms = t==C_CLIENT?10*1000:0;
  LOG(INFO)<<__func__<<" c="<<t<<" delay="<<delayms;
  _listen_thread->message_loop()->PostDelayedTask(FROM_HERE, base::Bind(&SrafIpcThread::initChannal_, base::Unretained(this)),base::TimeDelta::FromMilliseconds(delayms));
}
void SrafIpcThread::initChannal_(){
  LOG(INFO)<<__func__;
  IPC::ChannelHandle handle(_channal_name);
  //use MODE_OPEN_NAMED_SERVER instead of MODE_NAMED_SERVER , so server do not check client UID
  IPC::Channel::Mode mode = _channal_type==C_CLIENT?IPC::Channel::MODE_NAMED_CLIENT : IPC::Channel::MODE_OPEN_NAMED_SERVER ;
#if 0
  SrafSetUpSocket(&handle, mode);
  _channel.reset(new IPC::ChannelPosix(handle, mode, _listener));

  if(!_channel->Connect()){
    return ;
  }
#endif
  _sync_channel = IPC::SyncChannel::Create(handle, mode, _listener, _thread->task_runner().get(), true, new base::WaitableEvent(false, false)).release();
  _listener->setSender(_sync_channel);
  return ;
}




int SrafSetUpSocket(IPC::ChannelHandle *handle,IPC::Channel::Mode mode) {
  const std::string& name = handle->name;

  int socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
  if (socket_fd<0){
    return -1;
  }
  if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) < 0){
    return -2;
  }
  struct sockaddr_un server_address = { 0 };
  memset(&server_address, 0, sizeof(server_address));
  server_address.sun_family = AF_UNIX;
  int path_len = snprintf(server_address.sun_path, IPC::kMaxSocketNameLength,
                          "%s", name.c_str());
  size_t server_address_len = offsetof(struct sockaddr_un,
                                       sun_path) + path_len + 1;
  if (mode == IPC::Channel::MODE_NAMED_SERVER) {
    // Only one server at a time. Cleanup garbage if it exists.
    unlink(name.c_str());
    // Make sure the path we need exists.
    base::FilePath path(name);
    base::FilePath dir_path = path.DirName();
    if(!base::CreateDirectory(dir_path)){
      return -3;
    }
    if(bind(socket_fd,
                   reinterpret_cast<struct sockaddr *>(&server_address),
                   server_address_len) < 0){
      LOG(ERROR)<<__func__<< server_address.sun_path
                          << ": " << strerror(errno)<< "(" << errno << ")";
      return -4;
    }
    if(listen(socket_fd, SOMAXCONN) < 0){
      LOG(ERROR)<<__func__<< server_address.sun_path
                          << ": " << strerror(errno)<< "(" << errno << ")";
      return -5;
    }
  } else if (mode == IPC::Channel::MODE_NAMED_CLIENT) {
    if(connect(socket_fd,
                      reinterpret_cast<struct sockaddr *>(&server_address),
                      server_address_len) < 0){
      LOG(ERROR)<<__func__<< server_address.sun_path
                          << ": " << strerror(errno)<< "(" << errno << ")";
      return -6;
    }
  } else {
    LOG(ERROR)<<__func__ << "Unknown mode " << mode;
    return -7;
  }
  handle->socket.fd = socket_fd;
  return 0;
}


void sraf_nb_sleep(base::MessageLoop* loop , int64 ms) {
  base::TimeDelta delay = base::TimeDelta::FromMilliseconds(ms);
  // Post a quit task so that this loop eventually ends and we don't hang
  // in the case of a bad test. Usually, the run loop will quit sooner than
  // that because all tests use a IPCChannelPosixTestListener which quits the
  // current run loop on any channel activity.
  loop->task_runner()->PostDelayedTask(FROM_HERE, loop->QuitClosure(), delay);
  loop->Run();
}

void sraf_io_sleep(int64 ms) {
  base::MessageLoopForIO* loop = base::MessageLoopForIO::current();
  sraf_nb_sleep(loop,ms);
}



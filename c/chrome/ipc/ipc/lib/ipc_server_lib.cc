#include "ipc_server_lib.h"
#include <android/log.h>

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "IPC_TEST", __VA_ARGS__)

scoped_ptr<base::Thread> IPC_SERVER::thread_io;
scoped_ptr<base::Thread> IPC_SERVER::thread_listen;
std::map<std::string, IPC_SERVER *> global_async_map;

IPC::ChannelHandle IPC_SERVER::GetTestChannelHandle() {
  return IPC::ChannelHandle(""+name);
}

IPC_SERVER::IPC_SERVER(std::string name): event(false, false) {
  this->name = name;
}

void IPC_SERVER::init(std::string name) {
  if(!init_channel_already()) {
    this->name = name;
    if (IPC_SERVER::thread_io.get() == NULL) {
      thread_io.reset(new base::Thread(this->name+"_server"));
      base::Thread::Options options;
      options.message_loop_type = base::MessageLoop::TYPE_IO;
      thread_io->StartWithOptions(options);
    }
    if(IPC_SERVER::thread_listen.get() == NULL) {
      thread_listen.reset(new base::Thread(this->name+"_listen_server"));
      base::Thread::Options options_listen;
      options_listen.message_loop_type = base::MessageLoop::TYPE_IO;
      thread_listen->StartWithOptions(options_listen);
    }
    init_listen_thread_prepare();
  }
}

void IPC_SERVER::init_listen_thread_prepare() {
  thread_listen->task_runner()->PostTask(FROM_HERE, base::Bind(&IPC_SERVER::init_listen_thread, base::Unretained(this)));
  event.Wait();
}

void IPC_SERVER::init_listen_thread() {
  channel_proxy_ = IPC::ChannelProxy::Create(GetTestChannelHandle(), IPC::Channel::MODE_OPEN_NAMED_SERVER,
                                             this, thread_io->task_runner().get());
  event.Signal();
}

bool IPC_SERVER::Send(IPC::Message *message) {
  if(init_channel_already())
    return channel_proxy_->Send(message);
  LOGD("Please init ipc server first");
  return false;
}

bool IPC_SERVER::init_channel_already() {
  if(channel_proxy_.get())
    return true;
  return false;
}
std::string IPC_SERVER::getName() {
  return name;
}

void IPC_SERVER_SYNC::init_listen_thread_prepare() {
  thread_listen->task_runner()->PostTask(FROM_HERE, base::Bind(&IPC_SERVER_SYNC::init_listen_thread, base::Unretained(this)));
  event.Wait();
}

void IPC_SERVER_SYNC::init_listen_thread() {
  sync_channel_ = IPC::SyncChannel::Create(GetTestChannelHandle(), IPC::Channel::MODE_OPEN_NAMED_SERVER, this,
                                           thread_io->task_runner().get(), true, new base::WaitableEvent(false, false));
  event.Signal();
}

bool IPC_SERVER_SYNC::Send(IPC::Message *message) {
  if(init_channel_already())
    return sync_channel_->Send(message);
  LOGD("Please init ipc_sync_server first");
  return false;

}

bool IPC_SERVER_SYNC::init_channel_already() {
  if(sync_channel_.get())
    return true;
  return false;
}

IPC_SERVER_SYNC::IPC_SERVER_SYNC(): IPC_SERVER() {}
IPC_SERVER_SYNC::IPC_SERVER_SYNC(std::string pipeName): IPC_SERVER(pipeName) {}



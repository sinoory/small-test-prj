#include "ipc_client_lib.h"
#include <unistd.h>
#include <fcntl.h>
#include <android/log.h>

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "IPC_TEST", __VA_ARGS__)

scoped_ptr<base::Thread> IPC_CLIENT::thread_io;
scoped_ptr<base::Thread> IPC_CLIENT::thread_listen;

IPC_CLIENT::IPC_CLIENT(): event(false, false) {}

IPC_CLIENT::IPC_CLIENT(std::string pipeName): event(false, false) {
  this->name = name;
}

IPC::ChannelHandle IPC_CLIENT::GetTestChannelHandle() {
  return IPC::ChannelHandle(""+name);
}

void IPC_CLIENT::init(std::string name) {
  if(!init_channel_already()) {
    this->name = name;
    if(IPC_CLIENT::thread_io.get() == NULL) {
      thread_io.reset(new base::Thread(this->name+"_client"));
      base::Thread::Options options;
      options.message_loop_type = base::MessageLoop::TYPE_IO;
      thread_io->StartWithOptions(options);
    }
    if(IPC_CLIENT::thread_listen.get() == NULL) {
      thread_listen.reset(new base::Thread(this->name + "_listen_client"));
      base::Thread::Options options_listen;
      options_listen.message_loop_type = base::MessageLoop::TYPE_IO;
      thread_listen->StartWithOptions(options_listen);
    }
    init_listen_thread_prepare();
  }
}

void IPC_CLIENT::init_listen_thread_prepare() {
  thread_listen->task_runner()->PostTask(FROM_HERE, base::Bind(&IPC_CLIENT::init_listen_thread, base::Unretained(this)));
  event.Wait();
}

void IPC_CLIENT::init_listen_thread() {
  channel_proxy_ = IPC::ChannelProxy::Create(GetTestChannelHandle(), IPC::Channel::MODE_NAMED_CLIENT, this, thread_io->task_runner().get());
  event.Signal();
}

bool IPC_CLIENT::Send(IPC::Message *message) {
  return channel_proxy_->Send(message);
}

bool IPC_CLIENT::init_channel_already() {
  if(channel_proxy_.get())
    return true;
  return false;
}

void IPC_CLIENT_SYNC::init_listen_thread_prepare() {
  thread_listen->task_runner()->PostTask(FROM_HERE, base::Bind(&IPC_CLIENT_SYNC::init_listen_thread, base::Unretained(this)));
  event.Wait();
}

void IPC_CLIENT_SYNC::init_listen_thread() {
  sync_channel_ = IPC::SyncChannel::Create(GetTestChannelHandle(), IPC::Channel::MODE_NAMED_CLIENT, this, thread_io->task_runner().get(), true, new base::WaitableEvent(false, false));
  event.Signal();
}

bool IPC_CLIENT_SYNC::Send(IPC::Message *message) {
  bool ret = sync_channel_->Send(message);
  return ret;
}

bool IPC_CLIENT_SYNC::init_channel_already() {
  if(sync_channel_.get())
    return true;
  return false;
}

IPC_CLIENT_SYNC::IPC_CLIENT_SYNC(): IPC_CLIENT() {}
IPC_CLIENT_SYNC::IPC_CLIENT_SYNC(std::string pipeName): IPC_CLIENT(pipeName) {}

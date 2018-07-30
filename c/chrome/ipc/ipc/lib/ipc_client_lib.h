#ifndef __IPC_CLIENT_LIB_H__
#define __IPC_CLIENT_LIB_H__

#include "ipc_msg_lib.h"
#include "ipc/ipc_listener.h"
#include "ipc/ipc_channel_proxy.h"
#include "ipc/ipc_sync_channel.h"
#include "base/synchronization/waitable_event.h"

class __attribute__((visibility("default"))) IPC_CLIENT : public IPC::Listener {
public:
  IPC_CLIENT();
  IPC_CLIENT(std::string pipeName);
  virtual bool Send(IPC::Message *message);
  void init(std::string name);

protected:
  IPC::ChannelHandle GetTestChannelHandle();
  virtual void init_listen_thread_prepare();
  virtual void init_listen_thread();
  virtual bool init_channel_already();
  std::string name;
  base::WaitableEvent event;
  static scoped_ptr<base::Thread> thread_io;
  static scoped_ptr<base::Thread> thread_listen;
  scoped_ptr<IPC::ChannelProxy> channel_proxy_;
};

class __attribute__((visibility("default"))) IPC_CLIENT_SYNC : public IPC_CLIENT {
public:
  bool Send(IPC::Message *message);
  IPC_CLIENT_SYNC();
  IPC_CLIENT_SYNC(std::string pipeName);
protected:
  virtual void init_listen_thread_prepare();
  virtual void init_listen_thread();
  virtual bool init_channel_already();
  scoped_ptr<IPC::SyncChannel> sync_channel_;
};

#endif

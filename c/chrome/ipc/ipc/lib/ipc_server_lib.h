#ifndef __IPC_SERVER_LIB_H__
#define __IPC_SERVER_LIB_H__

#include "ipc/ipc_listener.h"
#include "ipc/ipc_channel_proxy.h"
#include "ipc/ipc_sync_channel.h"
#include "base/synchronization/waitable_event.h"
//#include "sraf_hbbtv_apm_HbbTVAPMService.h"


class __attribute__((visibility("default"))) IPC_SERVER : public IPC::Listener{

public:
  IPC_SERVER(): event(false, false) {}
  IPC_SERVER(std::string name);
  virtual bool Send(IPC::Message *message);
  std::string getName();
  virtual ~IPC_SERVER() {}
  void init(std::string name);

protected:
  bool OnMessageReceived(const IPC::Message& message) override {
    return true;
  }

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

class __attribute__((visibility("default"))) IPC_SERVER_SYNC : public IPC_SERVER{
public:
  bool Send(IPC::Message *message);
  IPC_SERVER_SYNC();
  IPC_SERVER_SYNC(std::string pipeName);

protected:
  virtual void init_listen_thread_prepare();
  virtual void init_listen_thread();
  virtual bool init_channel_already();
  scoped_ptr<IPC::SyncChannel> sync_channel_;
};

extern std::map<std::string, IPC_SERVER *> global_async_map;
#endif





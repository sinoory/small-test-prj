#ifndef __SRAF_LIB_IPC__
#define __SRAF_LIB_IPC__

#include "ipc/ipc_channel_posix.h"
#include "ipc/ipc_sync_channel.h"
#include "ipc/unix_domain_socket_util.h"

#define SRAF_SYM_VISIBLE __attribute__((visibility("default")))

enum EIPC{
  E_OK ,
};
class SrafIpcListener;
class SRAF_SYM_VISIBLE SrafIpcThread {
  public:
    enum EChannalType {
      C_SERVER,
      C_CLIENT,
    };
    SrafIpcThread(std::string threadname="ipc");
    ~SrafIpcThread();
    bool Send(IPC::Message *message);
    base::MessageLoop* _io_msg_loop;

    void initChannal(EChannalType t,std::string channal_name,SrafIpcListener* listener);
    scoped_ptr<IPC::ChannelPosix> _channel;
    scoped_ptr<base::Thread> _thread; 
    scoped_ptr<base::Thread> _listen_thread; 
    IPC::SyncChannel* _sync_channel;
  private:
    SrafIpcListener* _listener;
    EChannalType _channal_type;
    std::string _channal_name;
    std::string _thread_name;
    void initChannal_();
};

class SRAF_SYM_VISIBLE SrafIpcListener : public IPC::Listener {
  public:
    bool OnMessageReceived(const IPC::Message& message) override;

    bool Send(IPC::Message* message);
  private :
    friend SrafIpcThread;
    IPC::Sender* _sender;
    void setSender(IPC::Sender* sender);
};


#endif

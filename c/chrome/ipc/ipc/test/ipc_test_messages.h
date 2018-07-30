#ifndef SRAF_IPC_TEST_MSGS
#define SRAF_IPC_TEST_MSGS

#include <string>

#include "ipc/ipc_message_macros.h"
#define IPC_MESSAGE_START TestMsgStart

//async message, in 0 parame
IPC_MESSAGE_CONTROL0(Msg_C_0)
//async message, in 3 parame
IPC_MESSAGE_CONTROL3(Msg_C_3, int, std::string, bool)

//sync message , in 3 pararm, out 3 param
IPC_SYNC_MESSAGE_CONTROL3_3(Msg_C_3_3, int, std::string, bool, std::string,int, bool)
IPC_SYNC_MESSAGE_ROUTED3_3(Msg_R_3_3, int, std::string, bool, std::string,int, bool)

#endif

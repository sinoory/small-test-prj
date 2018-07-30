#ifndef __IPC_MSG_LIB_H__
#define __IPC_MSG_LIB_H__

#include "ipc/ipc_message_macros.h"
#include <string>

#define IPC_MESSAGE_START IPCTestMsgStart

IPC_MESSAGE_ROUTED2(IPC_MSG_Test_intro, int, std::string)
IPC_MESSAGE_ROUTED1(IPC_MSG_Test_Hello, std::string)
IPC_SYNC_MESSAGE_ROUTED2_1(IPC_SYNC_Test_intro, int, std::string, std::string)
IPC_SYNC_MESSAGE_ROUTED1_1(IPC_SYNC_Test_Hello, std::string, std::string)

#endif

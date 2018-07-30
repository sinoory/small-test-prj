#include "HbbTVAPMService_jni.h"
#include <jni.h>
#include "ipc_server_lib_jni.h"
#include <map>
#include "ipc_msg_lib.h"
#include "chrome/app/android/chrome_jni_onload.h"
#include <android/log.h>

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "IPC_TEST", __VA_ARGS__)

JavaVM* global_vm = NULL;
static std::map<std::string, jclass> global_class_map;

bool IPC_SERVER_JNI::OnMessageReceived(const IPC::Message& message) {
  LOGD("file %s func %s enter", __FILE__, __func__);
  IPC_BEGIN_MESSAGE_MAP(IPC_SERVER_JNI, message)
  IPC_MESSAGE_HANDLER(IPC_MSG_Test_Hello, OnHelloAsync)
  IPC_END_MESSAGE_MAP()
  return true;
}

void IPC_SERVER_JNI::OnHelloAsync(std::string str) {
  if(!global_vm) {
    LOGD("func %s IPC server init first!", __func__);
    return;
  }

  JNIEnv* env;
  global_vm->AttachCurrentThread(&env, NULL);
  jclass tmpClazz = NULL;
  if(global_class_map.find(getName()) != global_class_map.end()) {
    tmpClazz = global_class_map.find(getName())->second;
    jmethodID mid = env->GetStaticMethodID(tmpClazz, "MessageReceive", "(Ljava/lang/String;)I");
    if(!mid) {
       LOGD("func %s get method fail", __func__);
       return;
     }
     jstring tmpStr = env->NewStringUTF(str.c_str());
     env->CallStaticObjectMethod(tmpClazz, mid, tmpStr);
  }
  else {
    LOGD("func %s pipename %s get clazz fail!", __func__, getName().c_str());
  }
}

bool IPC_SERVER_JNI_SYNC::OnMessageReceived(const IPC::Message& message) {
  LOGD("file %s func %s enter", __FILE__, __func__);
  IPC_BEGIN_MESSAGE_MAP(IPC_SERVER_JNI_SYNC, message)
  IPC_MESSAGE_HANDLER(IPC_SYNC_Test_Hello, OnHello)
  IPC_END_MESSAGE_MAP()
  return true;
}

void IPC_SERVER_JNI_SYNC::OnHello(std::string str, std::string *value) {
  LOGD("func %s message %s receive", __func__, str.c_str());
  JNIEnv* env;
  global_vm->AttachCurrentThread(&env, NULL);
  jclass tmpClazz = NULL;

  if(global_class_map.find(getName()) != global_class_map.end()) {
    tmpClazz = global_class_map.find(getName())->second;
    jmethodID mid = env->GetStaticMethodID(tmpClazz, "SyncMessageReceive", "(Ljava/lang/String;)Ljava/lang/String;");
    if(!mid) {
       LOGD("func %s get method fail", __func__);
       return;
    }
    jstring tmpStr = env->NewStringUTF(str.c_str());
    jstring tmpValue = (jstring)env->CallStaticObjectMethod(tmpClazz, mid, tmpStr);
    jboolean flags = false;
    char * convertValue = (char *)env->GetStringUTFChars(tmpValue, &flags);
    *value = std::string(convertValue);
  }
  else {
    LOGD("func %s pipename %s get clazz fail!", __func__, getName().c_str());
  }
}

jint InitSyncIpcServer(JNIEnv* env, const JavaParamRef<jobject>& jcaller,
    const JavaParamRef<jstring>& pipeName) {

  jboolean flags = false;
  char *tmpName = (char *)env->GetStringUTFChars(pipeName, &flags);
  if(!tmpName) {
    LOGD("func %s convert name fail!", __func__);
    return -1;
  }
  IPC_SERVER_SYNC *tmp_server = new IPC_SERVER_JNI_SYNC();
  if(!tmp_server) {
    LOGD("func %s new IPC_SERVER fail", __func__);
    return -1;
  }
  tmp_server->init(tmpName);
  global_async_map.insert(std::pair<std::string, IPC_SERVER *>(tmpName, tmp_server));

  jclass tmp_clazz = env->GetObjectClass(jcaller);
  tmp_clazz = (jclass)env->NewGlobalRef(tmp_clazz);
  global_class_map.insert(std::pair<std::string, jclass>(tmpName, tmp_clazz));

  return 0;
}

jint InitIpcServer(JNIEnv* env, const JavaParamRef<jobject>& jcaller,
    const JavaParamRef<jstring>& pipeName) {

  jboolean flags = false;
  char *tmpName = (char *)env->GetStringUTFChars(pipeName, &flags);
  if(!tmpName) {
    LOGD("func %s convert name fail!", __func__);
    return -1;
  }

  IPC_SERVER_JNI * tmp_server = new IPC_SERVER_JNI();
  if(!tmp_server) {
    LOGD("func %s new IPC_SERVER fail", __func__);
    return -1;
  }
  tmp_server->init(tmpName);
  global_async_map.insert(std::pair<std::string, IPC_SERVER *>(tmpName, tmp_server));

  jclass tmp_clazz = env->GetObjectClass(jcaller);
  tmp_clazz = (jclass)env->NewGlobalRef(tmp_clazz);
  global_class_map.insert(std::pair<std::string, jclass>(tmpName, tmp_clazz));

  return 0;
}

jint SyncSendMessage(JNIEnv* env, const JavaParamRef<jobject>& jcaller,
    const JavaParamRef<jstring>& pipeName,
    const JavaParamRef<jstring>& message) {

  jboolean flags = false;
  IPC_SERVER * tmp_server = NULL;
  char *convertMessage = (char *)env->GetStringUTFChars(message, &flags);
  char *convertPipeName = (char *)env->GetStringUTFChars(pipeName, &flags);
  if(convertMessage == NULL || convertPipeName == NULL) {
    LOGD("func %s convert String NULL", __func__);
    return -1;
  }
  if(global_async_map.find(convertPipeName) != global_async_map.end()) {
    tmp_server = global_async_map.find(convertPipeName)->second;
    std::string tmpValue;
    IPC::Message *tmpMessage = new IPC_SYNC_Test_Hello(MSG_ROUTING_NONE, std::string(convertMessage), &tmpValue);
    if(!tmpMessage) {
      LOGD("func %s create message fail", __func__);
      return -1;
    }
    bool ret = tmp_server->Send(tmpMessage);
    if(!ret) {
      LOGD("func %s ret %d send message fail", __func__, ret);
    }
    else {
      jclass tmpClazz = env->GetObjectClass(jcaller);
      jmethodID mid = env->GetStaticMethodID(tmpClazz, "SyncMessageReceive", "(Ljava/lang/String;)Ljava/lang/String;");
      if(!mid) {
         LOGD("func %s get method fail", __func__);
         return -1;
       }
       jstring tmpStr = (jstring)env->NewStringUTF(tmpValue.c_str());
       env->CallStaticObjectMethod(tmpClazz, mid, tmpStr);
    }

    return (ret ? 0 : -1);
  }

  LOGD("func %s createIPCServer pipe %s first", __func__, convertPipeName);
  return -1;
}

jint SendMessage(JNIEnv* env, const JavaParamRef<jobject>& jcaller,
  const JavaParamRef<jstring>& pipeName, const JavaParamRef<jstring>& message) {

  jboolean flags = false;
  IPC_SERVER * tmp_server = NULL;
  char *convertMessage = (char *)env->GetStringUTFChars(message, &flags);
  char *convertPipeName = (char *)env->GetStringUTFChars(pipeName, &flags);
  if(convertMessage == NULL || convertPipeName == NULL) {
    LOGD("func %s convert String NULL", __func__);
    return -1;
  }

  if(global_async_map.find(convertPipeName) != global_async_map.end()) {
    tmp_server = global_async_map.find(convertPipeName)->second;
    IPC::Message *tmpMessage = new IPC_MSG_Test_Hello(MSG_ROUTING_NONE, std::string(convertMessage));
    if(!tmpMessage) {
      LOGD("func %s create message fail", __func__);
      return -1;
    }
    bool ret = tmp_server->Send(tmpMessage);
    if(!ret)
      LOGD("func %s ret %d send message fail", __func__, ret);

    return (ret ? 0 : -1);
  }

  LOGD("func %s createIPCServer pipe %s first", __func__, convertPipeName);
  return -1;
}

jint DestoryIpcServer(JNIEnv* env, const JavaParamRef<jobject>& jcaller,
    const JavaParamRef<jstring>& pipeName) {

  jboolean flags = false;
  char *convertPipeName = (char *)env->GetStringUTFChars(pipeName, &flags);
  if(convertPipeName == NULL) {
    LOGD("func %s convert string fail", __func__);
    return -1;
  }

  if(global_async_map.find(convertPipeName) != global_async_map.end()) {
    delete global_async_map.find(convertPipeName)->second;
    global_async_map.erase(convertPipeName);
    global_class_map.erase(convertPipeName);
  }

  return 0;
}

static bool RegisterJNI(JNIEnv* env) {
  RegisterNativesImpl(env);
  return true;
}

static bool Init() {
  return true;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
  std::vector<base::android::RegisterCallback> register_callbacks;
  std::vector<base::android::InitCallback> init_callbacks;

  global_vm = vm;
  register_callbacks.push_back(base::Bind(&RegisterJNI));
  init_callbacks.push_back(base::Bind(&Init));
  if (!base::android::OnJNIOnLoadRegisterJNI(vm, register_callbacks) ||
      !base::android::OnJNIOnLoadInit(init_callbacks)) {
    return -1;
  }
  return JNI_VERSION_1_4;
}

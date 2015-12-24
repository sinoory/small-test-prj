#include "stdio.h"  
#include "stdlib.h"  
#include "unistd.h"  
#include "pthread.h"  
#include <sys/syscall.h>  
#define gettid() syscall(__NR_gettid)  
#include <sys/types.h>
#include <unistd.h>

const int M = 3;  
pthread_t hThread[M];  
int threadId[M];  
void *hello(void *ptr) {  
        int id = *(int *)(ptr);  
        printf("thread pid=%d Thread id:%d\nn",getpid(), gettid());  
        sleep(1);  
          
}  
void initThreade() {  
        int i;  
        for (i = 0; i < M; ++i) {  
                    threadId[i] = i;  
                }  
}  
void MyThreads() {  
        int i;  
        for (i = 0; i < M; i++) {  
                    pthread_create(hThread + i, NULL, hello, (void *)(threadId + i));  
                }  
        for (i = 0; i < M; i++) {  
                    pthread_join(hThread[i], NULL);  
                }  
}  
int main(){  
        printf("Main  pid=%d Thread id:%d\nn",getpid(), gettid());  
        initThreade();  
        MyThreads();  
        return 0;  
}  


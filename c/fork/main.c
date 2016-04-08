#include <unistd.h>  
#include <stdio.h>   
#include "stdlib.h"  
#include "unistd.h"  
#include "pthread.h"  
#include <sys/syscall.h>  
#define gettid() syscall(__NR_gettid)  
#include <sys/types.h>
#include <unistd.h>



void parentf(){
    pid_t fpid=fork();   
    if (fpid < 0)   
    printf("error in fork!\n");   
    else if (fpid == 0) {  
        childf2();
    }  
    printf("p: my process id is %d\n",getpid());   
    sleep(10);
    printf("p: finish, my process id is %d\n",getpid());   


}

void childf(){
    printf("c: my process id is %d\n");   
    sleep(15);
    printf("c: finish, my process id is %d\n");   
}

void childf2(){
    printf("c2: my process id is %d\n",getpid());   
    sleep(15);
    printf("c2: finish, my process id is %d\n",getpid());   
}



int main ()   
{   
    pid_t fpid; 
    int count=0;  
    fpid=fork();   
    if (fpid < 0)   
    printf("error in fork!\n");   
    else if (fpid == 0) {  
        childf();
        count++;  
    }  
    else {  
        parentf();
        count++;  
    }  
    printf("统计结果是: %d\n",count);  
    return 0;  
}  

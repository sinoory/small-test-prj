#include <unistd.h>  
#include <stdio.h>   
#include "stdlib.h"  
#include "unistd.h"  
#include "pthread.h"  
#include <sys/syscall.h>  
#define gettid() syscall(__NR_gettid)  
#include <sys/types.h>
#include <unistd.h>

typedef void(*pfunc)();

void func(){
    printf("in func\n");
}

struct st_{
    int a;
    int b;
    pfunc pf;
};

int count=0;  
struct st_* pst;
void childf(){
    sleep(5);
    int oa=pst->a;
    count++;  
    pst->a=5;
    pst->pf();
    printf("c[%d]: finish, count=%d addr=%p padd=%p va=%d -> %d\n",getpid(),count,&count,pst,oa,pst->a);   
}

void childf2(){
    sleep(10);
    int oa=pst->a;
    count++;  
    pst->a=10;
    printf("c2[%d]: finish, count=%d addr=%p padd=%p va=%d -> %d\n",getpid(),count,&count,pst,oa,pst->a);   
}

void parentf(){
    pid_t fpid=fork();   
    if (fpid < 0)   
    printf("error in fork!\n");   
    else if (fpid == 0) {  
        childf2();
        return;
    }else{
        printf("parentf[%d] fork ok\n",getpid());
    }

}



int main ()   
{   
    pst=(struct st_*)malloc(sizeof(struct st_));
    pst->a=1;
    pst->b=1;
    pst->pf=&func;
    pid_t fpid; 
    fpid=fork();   
    if (fpid < 0)   
        printf("error in fork!\n");   
    else if (fpid == 0) {  
        childf();
        return;
    }  
    else {  
        parentf();
    }  
    printf("main count=: %d addr=%p paddr=%p\n",count,&count,pst);  
    return 0;  
}  

#include <stdio.h>
#include <errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<linux/input.h>

int simkey(int fd,int value)
{
    struct input_event event;
    event.type=EV_KEY;
    event.code=KEY_A; //模拟输入'A'
    event.value=value;
    gettimeofday(&event.time,0);

    if(write(fd,&event,sizeof(event))<0)
    {
        printf("simulate key error! \n");
        return -1;
    }

    return 0;
}

int main(){

    int fd_kbd; // /dev/input/event1
    int fd_mouse; //dev/input/mouse2


    fd_kbd = open("/dev/input/event0",O_RDWR);
    if(fd_kbd<=0){
        printf("error open keyboard:%s\n",strerror(errno));
        return -1;
    }
    if(simkey(fd_kbd,1)<0)
    {
        printf("No press !\n");
        return -1;
    }
    if(simkey(fd_kbd,0)<0)
    {
        printf("No release!\n");
        return -1;
    }

    close(fd_kbd);


    printf("YES!\n");

    return 0;
}


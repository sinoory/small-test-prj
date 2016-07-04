#include <stdio.h>

void testArrayLen(){
    printf("asdf\n");
    char* a[]={"asd","dkjkl","klk;l"};
    int len = sizeof(a)/sizeof(char*);
    printf("len a=%d\n",len);
}

void tststrcp(){
    char osrc[]="hello qq";
    char* src=osrc;

    char dst[20]={0};
    if(*src=='h' && *(src+1)=='e'){
        printf("first lett is he,will change to hi\n");
        *(src+1)='i';
    }

    strncpy(dst,src,5);
    printf("strncpy dst=%s\n",dst);
}

int main(){
    printf("addres of tststrcp=%p\n",tststrcp);
    tststrcp();
    return 0;
}

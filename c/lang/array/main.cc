#include <stdio.h>

void errArrayEmptyInit(){
    int a[]={};//error not point array size;
    for(int i=0;i<3;i++){
        a[i]=i+1;
    }
    for(int i=0;i<3;i++){
        printf("%s a[%d]=%d\n",__func__,i,a[i]);
    }
}

void okArrayEmptyInitWithSize(){
    int a[3]={};//ok with size;
    for(int i=0;i<3;i++){
        a[i]=i;
    }
    for(int i=0;i<3;i++){
        printf("%s a[%d]=%d\n",__func__,i,a[i]);
    }
}

void ohArrayEmptyInitWithoutSize(){
    int a[]={};//ok with size;
    a[0]=0;
    a[1]=1;
    a[2]=2;
    for(int i=0;i<3;i++){
        printf("%s a[%d]=%d\n",__func__,i,a[i]);
    }
}

void ArraySizeVarrable(int size){
    const int len=size;
    int a[len];//const varable for size ok
    a[0]=0;
    a[1]=1;
    a[2]=2;
    for(int i=0;i<3;i++){
        printf("%s a[%d]=%d\n",__func__,i,a[i]);
    }
}

int main(){
    errArrayEmptyInit();
    okArrayEmptyInitWithSize();
    //ohArrayEmptyInitWithoutSize();
    int len=2;
    len+=1;
    ArraySizeVarrable(len);
    return 0;
}

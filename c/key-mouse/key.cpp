  #include <stdio.h>
  #include <X11/extensions/XTest.h>
  #include <X11/Xlib.h>
  #include <unistd.h>
  #include <stdlib.h>
  #include <iostream>
  #include <chrono>
#include <ctime>
#include <iomanip>
#include <string>

  Display *dspopen(){   
      
      Display *dsp = XOpenDisplay(NULL);
      if(!dsp) {
          printf("open display failed\n");
          return NULL;
     }
     return dsp;
 }
 int presskey(Display *dsp,int s){  //键盘按
     if(dsp==NULL)
         return -1;
 //    KeySym keysym=XStringToKeysym(s);
     KeyCode key=XKeysymToKeycode(dsp,s);
     if(key==NoSymbol)
         return -1;
     XTestFakeKeyEvent(dsp,key,1,CurrentTime);
     XFlush(dsp);
     return 0;
 }
 int move(Display *dsp,int x,int y) //鼠标移动
 {
     if(0==XTestFakeMotionEvent(dsp,-1,x,y,CurrentTime))
     {
         printf("Cannot move!\n");
         return -1;
     }
     return 0;
 }
 int buttonpress(Display *dsp,int type) //鼠标按，type=1表示左键，3是右键，2是中键
 {
     if(0==XTestFakeButtonEvent(dsp,type,1,CurrentTime))
     {
         printf("press failed\n");
         return -1;
     }
     return 0;
 }
 int buttonrelease(Display *dsp,int type) //鼠标释放
 {
     if(0==XTestFakeButtonEvent(dsp,type,0,CurrentTime))
     {
         printf("release failed\n");
         return -1;
     }
     return 0;
 }
 int releasekey(Display *dsp,int s){ //键盘release
     if(dsp==NULL)
         return -1;
 //    KeySym keysym=XStringToKeysym(s);
     KeyCode key=XKeysymToKeycode(dsp,s);
     if(key==NoSymbol)
         return -1;
     XTestFakeKeyEvent(dsp,key,0,CurrentTime);
     XFlush(dsp);
     return 0;
 }
 void dspclose( Display *dsp ){
     if(dsp!=NULL){
         XCloseDisplay(dsp);
         
     }
 }
 void mousewheel(Display* dsp,int direct,int count,int wheelIntervalMs){
     //int direct=4;//4 up,5 down
     for(;count>0;count--){
         Display *dsp=dspopen(); //TODO: maybe there exist an api to flush the event out, whithout open/close dsp
         XTestFakeButtonEvent(dsp,direct,1,CurrentTime);
         XTestFakeButtonEvent(dsp,direct,0,CurrentTime);//release
         usleep(1000*wheelIntervalMs);
         dspclose(dsp);
     }
 }
// using namespace std;
int testWheel(int argc,char** argv){     

    int wait=5;
    if(argc<5){
        printf("Usage: main serialnum serialIntervalMs wheelIntervalMs wheelNums\n");
        printf("     : will start after %d s\n",wait);
        return 0;
    }

    usleep(1000*1000*wait);

    Display *dsp=0;//dspopen();
    int serialnum=atoi(argv[1]);
    int serialIntervalMs = atoi(argv[2]);
    int wheelIntervalMs = atoi(argv[3]);
    int wheelNums = atoi(argv[4]);
    while(serialnum-->0){
        mousewheel(dsp,5,wheelNums,wheelIntervalMs);
        //std::cout<<"";
        //auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        //std::cout<< std::put_time(std::localtime(&t), "%Y-%m-%d %H.%M.%S");
        //std::cout<<" next sleep "<<serialIntervalMs<<" ms"<<std::endl;
        usleep(1000*serialIntervalMs);
    }

    //dspclose(dsp);
    return 0;
}

int testLoopWheel(int argc,char** argv){     

    int wait=5;
    if(argc<6){
        printf("Usage: main serialnum serialIntervalMs wheelIntervalMs wheelNums loopcnt\n");
        printf("     : will start after %d s\n",wait);
        return 0;
    }

    usleep(1000*1000*wait);

    Display *dsp=0;//dspopen();
    int serialnum=atoi(argv[1]);
    int serialIntervalMs = atoi(argv[2]);
    int wheelIntervalMs = atoi(argv[3]);
    int wheelNums = atoi(argv[4]);
    int loopcnt = atoi(argv[5]);
    int tmpserialnum=serialnum;
    while(loopcnt-->0){
        serialnum=tmpserialnum;
        while(serialnum-->0){
            mousewheel(dsp,5,wheelNums,wheelIntervalMs);
            usleep(1000*serialIntervalMs);
        }
        serialnum=tmpserialnum;
        while(serialnum-->0){
            mousewheel(dsp,4,wheelNums,wheelIntervalMs);
            usleep(1000*serialIntervalMs);
        }
    }

    //dspclose(dsp);
    return 0;
}

void testGetPoint(int &x, int &y){
    Display *display;
    Window root;
    if ((display = XOpenDisplay(NULL)) == NULL) {
        fprintf (stderr, "Cannot open local X-display./n" );
        return ;
    }
    root = DefaultRootWindow(display);

    int tmp;unsigned int tmp2;
    Window fromroot, tmpwin;
    XQueryPointer(display, root, &fromroot, &tmpwin, &x, &y, &tmp, &tmp, &tmp2);

    XCloseDisplay(display);

}

void testMouseMove(int argc,char** argv){
    int wait=5;
    if(argc<1){
        printf("Usage: main serialnum serialIntervalMs wheelIntervalMs wheelNums\n");
        printf("     : will start after %d s\n",wait);
        return ;
    }

    usleep(1000*1000*wait);

    int x=300,y=300;
     testGetPoint(x,y);
    printf("testGetPoint x=%d y=%d\n",x,y);

    Display *dsp=dspopen(); 
    buttonpress(dsp,1);
    move(dsp,x,y+600);
    buttonrelease(dsp,1);
     dspclose(dsp);
}


 int main(int argc,char** argv){
     //testWheel(argc,argv);
     testLoopWheel(argc,argv);
     //testMouseMove(argc,argv);
     return 0;
 }

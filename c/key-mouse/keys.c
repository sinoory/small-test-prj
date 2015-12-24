 1 #include <stdio.h>
 2 #include <X11/extensions/XTest.h>
 3 #include <X11/Xlib.h>
 4 Display *dspopen(){   
 5     
 6     Display *dsp = XOpenDisplay(NULL);
 7     if(!dsp) {
 8         printf("open display failed\n");
 9         return NULL;
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
 //int main(){     //测试用的会在程序结束后，在光标前输出c
 //    Display *dsp=dspopen();
 //    presskey(dsp,'c');
 //    releasekey(dsp,'c');
 //    dspclose(dsp);
 //    return 0;
 //}

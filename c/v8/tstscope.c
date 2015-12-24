#include <v8.h>
using namespace v8;

Local<Value> func(){
    HandleScope scope(); //为此函数开始一个新的HandleScope
      Local<Value> s=String::New("test"); //创建一个句柄
      return s; //函数结束时HandleScope会被析构，Local句柄会被回收吧？
};

int main(void){
      HandleScope scope;
      Local<Value> s=func(); //函数结束后依然可以使用原来的Local句柄
      String::AsciiValue v(s); //字符串句柄中获取Ascii值
      printf("%s\n",*v); //局部定义的数据可以照常输出“test”
      return 0;
}

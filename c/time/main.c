
#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>
#include <string.h>
       #include <sys/types.h>
              #include <unistd.h>
void testmemcpy(void*dst,void*src,size_t n){
    memcpy(dst,src,n);
}

void timediff(){
    /*
    auto start=std::chrono::high_resolution_clock::now();
    //do_something();
    auto stop=std::chrono::high_resolution_clock::now();
    std::cout<<"do_something() took "<<std::chrono::duration<double,std::chrono::seconds>(stop-start).count()<<" seconds"<<std::endl;
    */
}

void test(){
    std::time_t t = std::time(NULL);
    std::tm tm = *std::localtime(&t);
    std::cout.imbue(std::locale("CHS"));
    //std::cout <<std::put_time(&tm,"%Y-%m-%d %H:%M:%S") << '\n';
}

 int main ()
 {
       using namespace std::chrono;
      
     auto now = steady_clock::now();

     #define LEN 1024*1024*3
     void* src=malloc(LEN);
     void* dst=malloc(LEN);

       steady_clock::time_point t1 = steady_clock::now();
      
        memcpy(dst,src,LEN-1);
       steady_clock::time_point t2 = steady_clock::now();
      
       //duration<milliseconds> time_span = duration_cast<milliseconds>(t2 - t1);
       duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
       //duration<milliseconds> time_span ;//= duration_cast<duration<milliseconds>>(t2 - t1);
     
     auto diff =  steady_clock::now()-now;
     auto sec = duration_cast<milliseconds>(diff);
     std::cout << "this program runs:" << sec.count() << " ms" << std::endl;
     std::cout << "pid="<< getpid()<<std::endl;
       std::cout << "It took me " << time_span.count()*1000 << " ms.";
       std::cout << std::endl;
      
       return 0;
 }

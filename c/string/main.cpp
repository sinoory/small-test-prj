#include <stdio.h>
#include <string>
#include <string.h>
#include <iostream>
using namespace std;
void testpointA(char* p){
    //p =  "hello";
    memcpy(p,"hello",5);
}
void testpointB(char** p){
}

void teststring(int len){
    return;
}

void testf(std::string f){
    std::size_t start=f.find("max-age=");
    std::size_t end =f.find(",",start);
    if(end==std::string::npos){
        end=f.size()-1;    
    }

    std::cout<<f.replace(start,end-start,"max-age=12345678987654321")<<" b="<<start<<" end="<<end<<std::endl;
}

void test_rep(){
    std::string f1="Cache-Control:max-age=30, must-revalidate";
    std::string f2="Cache-Control:max-age=30";
    std::string f3="Cache-Control:max-age=30 , must-revalidate";
    std::string f4="Cache-Control: must-revalidate,max-age=30,age";
    std::string f5="max-age=30, must-revalidate";
    testf(f1);
    testf(f2);
    testf(f3);
    testf(f4);
    testf(f5);
}


int main(){

    test_rep();
    return 0;
}


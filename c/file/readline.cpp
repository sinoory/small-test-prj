
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

int getSleepCnt(){
    using namespace std;
    ifstream   in("webprocessSleeps");  
    string   line;  
    getline(in,   line);  
    cout   << "line="<<  line << endl;   
    return atoi(line.c_str());

}
int main(){
    printf("getSleepCnt=%d\n",getSleepCnt());
}



#include <sstream>
#include <iostream>

using namespace std;

void testString(){
    ostringstream ostr;
    ostr<<"this is string 1"<<endl;
    cout<<ostr.str();

    ostr.str(""); //use this instead of clear() to clean ostr
    ostr<<"this is string after clear"<<endl;
    cout<<ostr.str();

    string s=string("hello")+" world";
    cout<<s<<endl;
}


int main(){
    testString();
}

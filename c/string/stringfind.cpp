#include <stdio.h>
#include <string>
#include <vector>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string.h>
using namespace std;
void findJsSrc1(const char* pcontent,int len,std::vector<std::string> & vs){
    const int max_line = 50;
    int curline=0,i,scripStart=-1,srcStart=-1,srcIndStart=-1,srcIndEnd=-1;
    for(i=0; i<len-10 ; i++){
        if(*(pcontent+i)=='\n'){
            curline++;
            if(curline >= max_line)
                break;
            continue;
        }
        if(scripStart==-1){
            if(*(pcontent+i)=='<'){
                if(!strncmp(pcontent+i,"<script",sizeof("<script")-1)){
                    scripStart=i;
                    i+=sizeof("<script")-1;
                }
            }
            continue;
        }
        if(srcStart==-1){
            if(*(pcontent+i)=='s'){
                if(!strncmp(pcontent+i,"src=",sizeof("src=")-1)){
                    srcStart=i;
                    i+=sizeof("src=")-2;
                }
            }
            continue;
        }
        if(srcIndStart==-1){
            if(*(pcontent+i)=='"'){
                srcIndStart=i;
            }
            continue;
        }
        if(srcIndEnd==-1){
            if(*(pcontent+i)=='"'){
                srcIndEnd=i;
                cout<<"find one js"<<std::string(pcontent+srcIndStart+1,srcIndEnd-srcIndStart-1)<<endl;
                //cout<<"p scripStart"<<scripStart<<" srcStart"<<srcStart<<" srcIndStart"<<srcIndStart<<" srcIndEnd"<<srcIndEnd<<endl;
                scripStart=srcStart=srcIndStart=srcIndEnd=-1;
            }
        }
    }
}
void findJsSrc(const char* pcontent,int len,std::vector<std::string> & vs){
    const int max_line = 80;
    int curline=0,i,scripStart=-1,srcStart=-1,srcIndStart=-1,srcIndEnd=-1;
    cout<<__func__<<"start len="<<len<<endl;
    for(i=0; i<len-10 ; i++){
        if(*(pcontent+i)=='\n'){
            curline++;
            if(curline >= max_line)
                break;
            continue;
        }
        if(scripStart==-1){
            if(*(pcontent+i)=='<'){
                if(!strncmp(pcontent+i,"<script",sizeof("<script")-1)){
                    scripStart=i;
                    i+=sizeof("<script")-1;
                }
            }
            continue;
        }
        if(srcStart==-1){
            if(*(pcontent+i)=='s'){
                if(!strncmp(pcontent+i,"src=",sizeof("src=")-1)){
                    srcStart=i;
                    i+=sizeof("src=")-2;
                }
            }
            continue;
        }
        if(srcIndStart==-1){
            if(*(pcontent+i)=='"'){
                srcIndStart=i;
            }
            continue;
        }
        if(srcIndEnd==-1){
            if(*(pcontent+i)=='"'){
                srcIndEnd=i;
                vs.push_back(std::string(pcontent+srcIndStart+1,srcIndEnd-srcIndStart-1));
                scripStart=srcStart=srcIndStart=srcIndEnd=-1;
            }
        }
    }
    cout<<__func__<<"finish line="<<curline<<" find "<<vs.size()<<" js"<<endl;
}


void ReadDataFromFileLBLIntoString()
{
    ifstream fin("data.txt");  
    string s,ss;  
    while( getline(fin,s) ){    
        ss=ss+s+"\n";
    }
    std::vector<std::string> vs;
    findJsSrc(ss.c_str(),ss.length(),vs);
    //findJsSrc("<script type=\"text/javascript\" src=\"xo1.js\" charset=\"UTF-8\"></script>",ss.length(),vs);
    //findJsSrc("<script async=\"\" src=\"./page/analytics.js\"></script>",ss.length(),vs);
    for (size_t i = 0; i < vs.size(); ++i){
        cout<<" js: "<<vs[i]<<endl;
    }
}
void spliteString(string domains){
    cout<<"-------"<<domains<<"-----"<<endl;
    string domain;
    int dstart=0,dend=0,len=domains.length();
    const char* p = domains.c_str();
    for(int i=0;i<len;i++){
        if(*(p+i)=='|'||i==len-1){
            dend=i;
            if(*(p+i-1)=='^'){
                dend-=1;
            }
            cout<<std::string(p+dstart,p+dend)<<endl;
            dstart=i+1;
        }
    }

}
int main(){
    string a="asdfa";
    a+='\0';
    a+='\0';
    a+='\0';
    a+="bbb";
    a+='\0';
    a+='\0';
    a+="ccc";
    cout<<"a="<<a;
    return 0;
    spliteString("http://1.com");
    spliteString("http://1.com^");
    spliteString("http://1.com^|https://2.com");
    spliteString("http://1.com^|https://2.com|");
    return 0;
}




#include "common.h"
#include "coded_stream.h"

int main(){
    google::protobuf::uint8 buf[20]={0};
    google::protobuf::io::CodedInputStream example(buf,20);
    return 0;
}

#include <string>
#include <iostream>

int main(){

  const char* res_dir="resdir";
  std::string langFileName;
  langFileName.assign(res_dir);
  langFileName.append("rc/");
    langFileName.clear();
    langFileName.append(res_dir);
    langFileName.append("/rc/en.xml");

  std::cout<<langFileName<<"\n";

}

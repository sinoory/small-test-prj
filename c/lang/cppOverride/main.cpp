# include <iostream> 
# include <tr1/memory>  
using namespace std;  
class B{
    public:
    int Status;
    virtual void f()=0;
};
class D:B{
    public:
    enum Status{
        AAA,
        BBB
    };

    virtual void f(D::Status s) ;
    virtual void f();// override;
    char iStatus;//name to Status will cause compile error

};
#if 1
void D::f() {
    return;
}
#endif
void D::f(D::Status s){
    return;
}

void stdptrp(std::unique_ptr<D> p){}

void tststdptr(){
    D* pd=new D();
    std::unique_ptr<D> p(pd);
    stdptrp(std::move(p));
}


int main(){
    #define myshptr std::tr1::shared_ptr  //ok
    D* d=new D();
    std::tr1::shared_ptr<D> ptrB1(new D()); 
    myshptr<D> ptrB2(new D()); 
    int i=D::AAA;
    //d->f();
}


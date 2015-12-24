
# include <iostream>
using namespace std;

template <class T>
class TC{
    public:
    T* func(T* t){return t;}
};

class A{
    public:
    int m;
};

class B{
    public:
    int m;
};

class C{
    public:
    int m;
};

class D : public A{
    public:
    int j;
};




template <class T>
void pf(TC<T> tc){
    cout<<"in pf\n";
}

void pf(TC<A> tc){
    cout<<"in pf a\n";
}

void pf(TC<B> tc){
    cout<<"in pf b\n";
}

int main(){
    A a;
    TC<D> tc;
    pf(tc);
    return 0;
}


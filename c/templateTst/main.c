#include <stdio.h>

template<typename T> 
class RefPtr {
    public:
        RefPtr()  { 
            printf("RefPtr()\n"); 
        }
        RefPtr(T* ptr) : m_ptr(ptr) {  
            printf("RefPtr(T*)\n"); 
        }
        RefPtr& operator=(T*);
    RefPtr& operator=( RefPtr&);
    //RefPtr& operator=(const RefPtr&);

    private:
        T* m_ptr;
};

template<typename T> inline RefPtr<T>& RefPtr<T>::operator=(T* optr)
{
        m_ptr=optr; 
    printf("RefPtr::operator=T*\n"); 
        return *this;
}

template<typename T> inline RefPtr<T>& RefPtr<T>::operator=( RefPtr& o)
{
    printf("RefPtr::operator=RefPtr\n"); 
        return *this;
}


class A{
    private:
        int x;
};
void testclasstmp(){
    RefPtr<A> p0;
    RefPtr<A> p=new A();
    RefPtr<A> p1(new A());
    printf("next p1=p\n");
    p1=p;
}

//////////////////test template function point//////////////
typedef void (*pfun)();

template<int T>
void testfunc(){
    printf("ok testfunc %d\n",T);
}


void testpointfunc(){
    int i=0;
    pfun pf=testfunc<i>;
    pf();
}



int main(){
    testpointfunc();
    return 0;
}

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

class B{
    public:
	void nf(){cout<<"B:nf()\n";}
	virtual void vf(){cout<<"B:vf()\n";}
	static void sfa(){cout<<"B:sfa()\n";}
	static void sfb(){cout<<"B:sfb()\n";sfa();}
};

class D:public B{
    public:
	void nf(){cout<<"D:nf()\n";}
	void vf(){cout<<"D:vf()\n";}
	static void sfa(){cout<<"D:sfa()\n";}
	static void sfb(){cout<<"D:sfb()\n";sfa();}
};

#define GEN_N(x) #x

void testVirtual(){
    char* s=GEN_N(58);
	B* p=new D();
	p->nf();
	p->vf();
	D* pd=new D();
	pd->nf();

	B::sfb();
    cout<<"test static virutal\n";
	p->sfb();
    cout<<"test none virutal\n";
	p->nf();
}

int main(){
    //testString();
	testVirtual();
}

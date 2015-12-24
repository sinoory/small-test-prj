

typedef struct ST{
    int i;
    void (*test)(struct ST* s);
};

typedef struct Student
{
    int a;
}Stu;

struct  sTest{
        int i;
      int (*conn) (struct  sTest *st,int *a, char *b);        //函数指针
      int (*create)(struct  sTest *st,int *a, long b);
};

int main(){
    Stu* s;
    struct ST t;
    struct STest* sa;
    return 0;
}

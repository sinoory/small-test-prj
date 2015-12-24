#include <stdio.h>
#include <locale.h>
#include <wchar.h>
int main(void) {
        //setlocale(LC_CTYPE, "");
        char str[] = "中文";
        printf("1:%s\n", str);
        return 0;
}

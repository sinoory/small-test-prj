#include<stdio.h>

#include <termios.h>
#include <unistd.h>
void setchar(){
    int ch, i;
    struct termios term, ot;

    tcgetattr( STDIN_FILENO, &term );
    ot=term;
    term.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &term );

    for (i = 0; i < 4; i++) {
        puts("enter arrow");
        ch = getchar() & 0xFF;
        ch += getchar() & 0xFF;
        ch += getchar() & 0xFF;
        printf("%c | %d\n", ch, ch); 
    }
    tcsetattr( STDIN_FILENO, TCSANOW, &ot );//retore setting
}

//exit if not arrow key
int arrowkey(){
    int ch, i;
    struct termios term, ot;

    puts("enter arrow key mode, exit if not arrow key");
    tcgetattr( STDIN_FILENO, &term );
    ot=term;
    term.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &term );

    while (1) {
        ch = getchar() & 0xFF;
        //printf("getchar= %d\n",ch);
        if(ch==10){
          printf("process ok %d\n",ch);
          continue;
        }else if(ch != 27) {
          printf("invalid char %d\n",ch);
          break;
        }
        ch = getchar() & 0xFF;
        if(ch != 91) {
          printf("invalid char %d\n",ch);
          break;
        }
        ch = getchar() & 0xFF;
        printf("process arrow %d\n",ch);
    }
    tcsetattr( STDIN_FILENO, TCSANOW, &ot );//retore setting
}


void tstcha( ){
        int c;
        char ca[20];
        //scanf("%1s",ca);
        while(1){
            c=getchar();
            printf ("%c\n", c);
            if(c=='\n'){
                printf("bye");
                return;
            }
        }
        while( (c=getchar()) != '\n' )
            printf ("%c", c);
}


void main(){
    arrowkey();
}

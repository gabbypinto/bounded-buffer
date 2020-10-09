#include <stdio.h>
#include <unistd.h>
#include <signal.h>
void sig_handler(int sig){

    printf("%s:: Got the Signal %d \n", __FUNCTION__, sig);

}
int main(){
    struct sigaction act;

    act.sa_handler = sig_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    sigaction(SIGINT, &act, 0);

    while (1) {
        printf("Hello World \n");
        sleep(1);
    }

    return 0;

}

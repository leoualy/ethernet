#include <stdio.h>
#include <sys/socket.h>
#include <net/ethernet.h>

int main(){
    // sock_raw
    int fd=socket(PF_NDRV,SOCK_RAW,htons(ETHERTYPE_IP));
    printf("fd listen is %d\n",fd);
    return 0;
}

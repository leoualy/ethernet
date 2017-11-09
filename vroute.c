#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/if_arp.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netpacket/packet.h>
#include <pthread.h>
#define BUF_SIZE 2048

int socket_raw();
void catch_packet();



int main(){
    catch_packet();
    return 0;
}

int socket_raw(){
        return socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
}
void catch_packet(){
    struct iphdr *pIPHdr;
    struct ethhdr *ehdr;
    int rcvLen;
    char buf[BUF_SIZE];
    int fd=socket_raw();
    while(1){
        rcvLen=recvfrom(fd,buf,BUF_SIZE,0,NULL,NULL);
        ehdr=(struct ethhdr *)buf;
        int protol_type=ntohs(ehdr->h_proto);
        if(0x800!=protol_type){
            continue;
        }
        pIPHdr=(struct iphdr *)(buf+sizeof(struct ethhdr));
        printf("type is %d\n",protol_type);
        unsigned int  saddr=pIPHdr->saddr;
        unsigned int  daddr=pIPHdr->daddr;
        struct in_addr sin,din;
        memcpy(&sin,&saddr,4);
        memcpy(&din,&daddr,4);
        char *pcaddr=inet_ntoa(sin);
        printf("src id is %s",pcaddr);
        printf(",");
        printf("des ip is %s\n",inet_ntoa(din));
        sleep(1);
        
        // printf("total is %d\n",pIPHdr->protocol);
        
        // printf("mac src: ");
        // for(rcvLen=0;rcvLen<6;rcvLen++){
        // printf("%x:",ehdr->h_source[rcvLen]);
        // }
        // 
        // printf("mac des: ");
        // for(rcvLen=0;rcvLen<6;rcvLen++){
        // printf("%x:",ehdr->h_dest[rcvLen]);
        // }
        // printf("\n");
     }
}

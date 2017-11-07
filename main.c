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
#include <errno.h>
#define IP_ADDR_LEN 4
#define BUF_SIZE 2048
#define ETH_ARP_LEN sizeof(struct ether_arp)
#define ETH_HDR_LEN sizeof(struct ether_header)
#define ETH_PACKET_LEN ETH_HDR_LEN+ETH_ARP_LEN 
#define DES_MAC {0x3c,0xe5,0xa6,0x58,0x9b,0xdd}
#define SRC_IP "192.168.2.10"
#define DES_IP "192.168.2.1"
int main1();
int socket_raw(){
        return socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
}

void fill_arp(char *buf,
        unsigned char *src_mac,unsigned char *des_mac,
        char *src_ip,char *des_ip){
    struct in_addr src_inaddr,des_inaddr; 
    struct ether_arp *parp=(struct ether_arp*)(buf+ETH_HDR_LEN);
    parp->ea_hdr.ar_hrd=htons(ARPHRD_ETHER);
    parp->ea_hdr.ar_pro=htons(ETHERTYPE_IP);
    parp->ea_hdr.ar_op=htons(ARPOP_REPLY);
    parp->ea_hdr.ar_hln=ETH_ALEN;
    parp->ea_hdr.ar_pln=4;
    inet_pton(AF_INET,src_ip,&src_inaddr);
    inet_pton(AF_INET,des_ip,&des_inaddr);    
    memcpy(parp->arp_sha,src_mac,ETH_ALEN);
    memcpy(parp->arp_tha,des_mac,ETH_ALEN);
    memcpy(parp->arp_spa,&src_inaddr,IP_ADDR_LEN);
    memcpy(parp->arp_tpa,&des_inaddr,IP_ADDR_LEN); 
}

void fill_eth_hdr(char *buf,
		unsigned char *src_mac,unsigned char *des_mac){
    struct ether_header *pethhdr;
    pethhdr = (struct ether_header *)buf;
    memcpy(pethhdr->ether_dhost,des_mac,ETH_ALEN);
    memcpy(pethhdr->ether_shost,src_mac,ETH_ALEN);
    pethhdr->ether_type=htons(ETHERTYPE_ARP);
}

int main(){
    char buf[ETH_PACKET_LEN];
    unsigned char src_mac[ETH_ALEN];
    unsigned char des_mac[ETH_ALEN]=DES_MAC;
    struct sockaddr_ll saddr_ll;
    
    int fd=socket_raw();
    printf("fd is %d\n",fd);
    
    struct ifreq ifr;
    strcpy(ifr.ifr_ifrn.ifrn_name,"eth1");
    ioctl(fd,SIOCGIFINDEX,&ifr);
    int iIFIndex=ifr.ifr_ifru.ifru_ivalue;
    ioctl(fd,SIOCGIFHWADDR,&ifr);
    memcpy(src_mac,ifr.ifr_ifru.ifru_hwaddr.sa_data,ETH_ALEN);

    saddr_ll.sll_ifindex=iIFIndex;
    saddr_ll.sll_family=PF_PACKET;
    // ethernet header    
    fill_eth_hdr(buf,src_mac,des_mac);
    // arp 
    fill_arp(buf,src_mac,des_mac,(char *)SRC_IP,(char *)DES_IP);
    int len=ETH_PACKET_LEN;
    printf("len is %d\n",len);
    printf("before sendto error:%s\n",strerror(errno));
    socklen_t add_len=sizeof(struct sockaddr_ll);
    while(1){
	printf("fd is %d\n",fd);
	
        int retlen = sendto(fd,buf,ETH_PACKET_LEN,0,(const struct sockaddr *)&saddr_ll,add_len);
        if(retlen<0){
		printf("error:%s\n",strerror(errno));
		break;
	}
	printf("retlen is %d\n",retlen);

	// sleep(2);
    }
    return 0;
}

// void catch_packet(){
//     struct iphdr *pIPHdr;
//     struct ethhdr *ehdr;
//     int rcvLen;
//     char buf[BUF_SIZE];
//     int fd=socket_raw();
//     printf("raw fd is %d\n",fd);
//     while(1){
//        rcvLen=recvfrom(fd,buf,BUF_SIZE,0,NULL,NULL);
//        ehdr=(struct ethhdr *)buf;
//        pIPHdr=(struct iphdr *)(buf+sizeof(struct ethhdr));
//        printf("total is %d\n",pIPHdr->protocol);
//        printf("mac src: ");
//        for(rcvLen=0;rcvLen<6;rcvLen++){
//        printf("%x:",ehdr->h_source[rcvLen]);
//        }
//        
//        printf("mac des: ");
//        for(rcvLen=0;rcvLen<6;rcvLen++){
//        printf("%x:",ehdr->h_dest[rcvLen]);
//        }
//        printf("\n");
//     }
// 
// 
// }

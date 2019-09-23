#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
#include "raw.h"
#include "struct.h"

#define PROTO_UDP	17
#define DST_PORT	8000

SERVER serverChannels;
char this_mac[6];
char bcast_mac[6] =	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
char dst_mac[6] =	{0xa4, 0x1f, 0x72, 0xf5, 0x90, 0xb7};
char src_mac[6] =	{0xa4, 0x1f, 0x72, 0xf5, 0x90, 0xb7};

struct ifreq ifopts, if_mac,if_idx ;
struct sockaddr_in serverAddr;
char ifName[IFNAMSIZ];
int serverSocket, numbytes;
char *p;
struct sockaddr_ll socket_address;
uint8_t msg[80];

union eth_buffer buffer_u;

uint32_t ipchksum(uint8_t *packet)
{
    uint32_t sum=0;
    uint16_t i;

    for(i = 0; i < 20; i += 2)
        sum += ((uint32_t)packet[i] << 8) | (uint32_t)packet[i + 1];
    while (sum & 0xffff0000)
        sum = (sum & 0xffff) + (sum >> 16);
    return sum;
}

void loop();
void sendMessage(uint8_t ip[4], char *mensage);

int main(int argc, char *argv[])
{
    serverChannels.primeiro = NULL;
    serverChannels.ultimo = NULL;
    serverChannels.numDeCanais = 0;
    serverChannels.clientesServidor = NULL;
    /* Get interface name */
    if (argc > 1)
        strcpy(ifName, argv[1]);
    else
        strcpy(ifName, DEFAULT_IF);

    /* Open RAW socket */
    if ((serverSocket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1)
        perror("socket");

    printf("Iniciando servidor!\n");
    /* Set interface to promiscuous mode */
    strncpy(ifopts.ifr_name, ifName, IFNAMSIZ-1);
    ioctl(serverSocket, SIOCGIFFLAGS, &ifopts);
    ifopts.ifr_flags |= IFF_PROMISC;
    ioctl(serverSocket, SIOCSIFFLAGS, &ifopts);

    /* End of configuration. Now we can receive data using raw sockets. */
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8000);
    serverAddr.sin_addr.s_addr = inet_addr("10.0.2.15");

    /*if(bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0){
    	perror("bind");
    	return 0;
    }

    if (listen(serverSocket, 5) < 0) {
        perror("listen");
        exit(1);
    }*/
    loop();

    return 0;
}

void loop()
{
    int i;
	char *input;
	char aux[15];
    uint8_t ip[4];

    /*c = malloc(sizeof(CLIENTE));
    if (c == NULL)
    {
        printf("ERRO ALOCAÇÃO CLIENTE!\n");
        return NULL;
    }
    strncpy ( c->name, "new", 3);*/

    while (1) {

        numbytes = recvfrom(serverSocket, buffer_u.raw_data, ETH_LEN, 0, NULL, NULL);
        if (buffer_u.cooked_data.ethernet.eth_type == ntohs(ETH_P_IP) ) { //&& ((buffer_u.cooked_data.payload.ip.dst[0] == 10)&&(buffer_u.cooked_data.payload.ip.dst[1]==32)&&(buffer_u.cooked_data.payload.ip.dst[2]==143)&&(buffer_u.cooked_data.payload.ip.dst[3]==83)
            /*printf("IP packet, %d bytes - src ip: %d.%d.%d.%d dst ip: %d.%d.%d.%d proto: %d\n",
            	numbytes,
            	buffer_u.cooked_data.payload.ip.src[0], buffer_u.cooked_data.payload.ip.src[1],
            	buffer_u.cooked_data.payload.ip.src[2], buffer_u.cooked_data.payload.ip.src[3],
            	buffer_u.cooked_data.payload.ip.dst[0], buffer_u.cooked_data.payload.ip.dst[1],
            	buffer_u.cooked_data.payload.ip.dst[2], buffer_u.cooked_data.payload.ip.dst[3],
            	buffer_u.cooked_data.payload.ip.proto
            );*/
            if (buffer_u.cooked_data.payload.ip.proto == PROTO_UDP && buffer_u.cooked_data.payload.udp.udphdr.dst_port == ntohs(DST_PORT)) {
                p = (char *)&buffer_u.cooked_data.payload.udp.udphdr + ntohs(buffer_u.cooked_data.payload.udp.udphdr.udp_len);
                *p = '\0';
                printf("src port: %d dst port: %d size: %d msg: %s ip:%d.%d.%d.%d\n",
                       ntohs(buffer_u.cooked_data.payload.udp.udphdr.src_port), ntohs(buffer_u.cooked_data.payload.udp.udphdr.dst_port),
                       ntohs(buffer_u.cooked_data.payload.udp.udphdr.udp_len), (char *)&buffer_u.cooked_data.payload.udp.udphdr + sizeof(struct udp_hdr) , buffer_u.cooked_data.payload.ip.src[0], buffer_u.cooked_data.payload.ip.src[1],
            	buffer_u.cooked_data.payload.ip.src[2], buffer_u.cooked_data.payload.ip.src[3]
                      );

                ip[0] = buffer_u.cooked_data.payload.ip.src[0];
                ip[1] = buffer_u.cooked_data.payload.ip.src[1];
                ip[2] = buffer_u.cooked_data.payload.ip.src[2];
                ip[3] = buffer_u.cooked_data.payload.ip.src[3];
                
                CLIENTE *c = retornaCliente(ip, &serverChannels);
               // c->ip[0] = ip[0];
               // c->ip[1] = ip[1];
               // c->ip[2] = ip[2];
               // c->ip[3] = ip[3];
                //echo send
			 	input = (char *)&buffer_u.cooked_data.payload.udp.udphdr + sizeof(struct udp_hdr);

                memset(aux, '\0', sizeof(aux));
                if(strncmp(input,"/NICK",5)==0) {
                    memset(c->name, '\0', sizeof(c->name));
                    strncpy ( c->name, &input[6], strlen(input) - 6 );
                    printf("\nUsuário alterou o  nick para %s!\n", c->name);
                    sprintf(msg, "Nick alterado!");
				} 
                else if(strncmp(input,"/CREATE",7)==0) {
                    strncpy ( aux, &input[8], strlen(input) - 8);
                    printf("\nCriando canal %s !!!\n", aux);
					criarCanal(aux, c, &serverChannels);
                    sprintf(msg, "Canal Criado!");
                }
                 else if(strncmp(input,"/REMOVE",7)==0) {
                    strncpy ( aux, &input[8], strlen(input) - 8 );
                    i = removeCanal(aux, c, &serverChannels);
                    if(i == 1)
                    {
                        sprintf(msg, "Canal Foi removido com Sucesso!");
                    }
                    else if(i== 0)
                    {
                        sprintf(msg, "Este canal não existe!");
                    }
                    else
                    {
                        sprintf(msg, "Você não é o admistrador!");
                    }
                }
                 else if(strncmp(input,"/LIST",5)==0) {
                    memset(msg, '\0', sizeof(msg));
					LISTC *list = serverChannels.primeiro;
                    if(serverChannels.numDeCanais != 0){
                        while(list != NULL)
                        {
                            strcat(msg, list->chnl->name);
                            printf("%s\n", list->chnl->name);
                            msg[strlen(msg)] = '\n';
                            list = list->prox;
                        }
                            msg[strlen(msg)] = '\0';
                    }
                    else{
                        sprintf(msg, "Não há canais atualmente!");
                    }
                 }
				 else if(strncmp(input,"/JOIN",5)==0) {
                    strncpy ( aux, &input[6], strlen(input) - 6 );
                    if(c->channel != NULL)
                    {
                        sprintf(msg, "Você já está em um canal!");
                    }
                    else
                    {
                        CANAL *channel = retornaCanal(aux, &serverChannels);
                        if(channel == NULL)
                        {
                            sprintf(msg, "Canal não encontrado!");
                        }
                        else
                        {
                            c->channel = channel;
                            adicionaParticipante(c, channel);
                            sprintf(msg, "Você entrou!");
                        }
                    }
                } 
                else if(strncmp(input,"/PART",5)==0) {
                    if(c->channel == NULL)
                    {
                        sprintf(msg, "Você não está em um canal!");
                    }
                    sairDoCanal(c);
                    sprintf(msg, "Você saiu!");

                } 
                else if(strncmp(input,"/NAMES",6)==0) {
                    if(c->channel == NULL)
                    {
                        sprintf(msg, "Você não está em um canal!");
                    }
                    else
                    {
                        memset(msg, '\0', sizeof(msg));
                        LISTP *list = c->channel->primeiro;
                        while(list != NULL)
                        {
                            if(strncmp(list->clt->name,c->channel->admin->name,sizeof(c->channel->admin->name))==0)
                            {
                                msg[strlen(msg)] = '*';
                            }

                            strcat(msg, list->clt->name);
                            msg[strlen(msg)] = '\n';
                            list = list->prox;
                        }
                        msg[strlen(msg)] = '\0';                        
                    }
                } 
                else if(strncmp(input,"/KICK",5)==0) {
                    strncpy ( aux, &input[6], strlen(input) - 6 );
                    i = kickParticipante(aux, c, &serverChannels);
                    if(i == 1)
                    {
                        sprintf(msg, "Cliente %s foi removido do Canal!", aux);
                    }
                    else if(i== 0)
                    {
                        sprintf(msg, "Cliente não encontrado!");
                    }
                    else
                    {
                        sprintf(msg, "Você não é o admistrador!");
                    }
                } 
                /*else if(strncmp(input,"/msg",4)==0) {
                    strncpy ( aux, &input[5], strlen(input) - 5 );
                    sprintf(msg, "/MSG %s", aux);
                    printf(">>PRIVMSG !!!\n");
                    //if(strlen(str2) != 0) {
                    //	sprintf(echoString, "PRIVMSG %s %s", str2, str3);
                    //}
*/              else if(strncmp(input,"/QUIT",5)==0) {
                    sprintf(msg, "/QUIT");
                    if(c->channel != NULL)
                    {
                        sairDoCanal(c);
                    }
                    removeCliente(c, &serverChannels);
                    printf("Fechando o chat para %s!\n", c->name);
                 }
                 else
                 {
                    sprintf(msg, input);
                    if(c->channel != NULL){
                        LISTP *a;
                        a = c->channel->primeiro;
                        while(a != NULL)
                        {
                            sendMessage(a->clt->ip, msg);
                            a = a->prox;
                        }
                        continue;
                    
                    }
                    
                 }

                sendMessage(c->ip, msg);

            }
            continue;
        }

        //printf("got a packet, %d bytes\n", numbytes);
    }
}

void sendMessage(uint8_t ip[4], char *mensage)
{
    /* Get the index of the interface */
    memset(&if_idx, 0, sizeof(struct ifreq));
    strncpy(if_idx.ifr_name, ifName, IFNAMSIZ-1);
    if (ioctl(serverSocket, SIOCGIFINDEX, &if_idx) < 0)
        perror("SIOCGIFINDEX");
    socket_address.sll_ifindex = if_idx.ifr_ifindex;
    socket_address.sll_halen = ETH_ALEN;

    /* Get the MAC address of the interface */
    memset(&if_mac, 0, sizeof(struct ifreq));
    strncpy(if_mac.ifr_name, ifName, IFNAMSIZ-1);
    if (ioctl(serverSocket, SIOCGIFHWADDR, &if_mac) < 0)
        perror("SIOCGIFHWADDR");
    memcpy(this_mac, if_mac.ifr_hwaddr.sa_data, 6);

    /* End of configuration. Now we can send data using raw sockets. */

    /* Fill the Ethernet frame header */
    memcpy(buffer_u.cooked_data.ethernet.dst_addr, dst_mac, 6);
    memcpy(buffer_u.cooked_data.ethernet.src_addr, src_mac, 6);
    buffer_u.cooked_data.ethernet.eth_type = htons(ETH_P_IP);

    /* Fill IP header data. Fill all fields and a zeroed CRC field, then update the CRC! */
    buffer_u.cooked_data.payload.ip.ver = 0x45;
    buffer_u.cooked_data.payload.ip.tos = 0x00;
    buffer_u.cooked_data.payload.ip.len = htons(sizeof(struct ip_hdr) + sizeof(struct udp_hdr) + strlen(msg));
    buffer_u.cooked_data.payload.ip.id = htons(0x00);
    buffer_u.cooked_data.payload.ip.off = htons(0x00);
    buffer_u.cooked_data.payload.ip.ttl = 50;
    buffer_u.cooked_data.payload.ip.proto = 17; //0xff;
    buffer_u.cooked_data.payload.ip.sum = htons(0x0000);

    buffer_u.cooked_data.payload.ip.src[0] = 10;
    buffer_u.cooked_data.payload.ip.src[1] = 0;
    buffer_u.cooked_data.payload.ip.src[2] = 2;
    buffer_u.cooked_data.payload.ip.src[3] = 15;
    buffer_u.cooked_data.payload.ip.dst[0] = ip[0];
    buffer_u.cooked_data.payload.ip.dst[1] = ip[1];
    buffer_u.cooked_data.payload.ip.dst[2] = ip[2];
    buffer_u.cooked_data.payload.ip.dst[3] = ip[3];
    buffer_u.cooked_data.payload.ip.sum = htons((~ipchksum((uint8_t *)&buffer_u.cooked_data.payload.ip) & 0xffff));

    /* Fill UDP header */
    buffer_u.cooked_data.payload.udp.udphdr.src_port = htons(8000);
    buffer_u.cooked_data.payload.udp.udphdr.dst_port = htons(8000);
    buffer_u.cooked_data.payload.udp.udphdr.udp_len = htons(sizeof(struct udp_hdr) + strlen(msg));
    buffer_u.cooked_data.payload.udp.udphdr.udp_chksum = 0;

    /* Fill UDP payload */
    memcpy(buffer_u.raw_data + sizeof(struct eth_hdr) + sizeof(struct ip_hdr) + sizeof(struct udp_hdr), msg, strlen(msg));

    /* Send it.. */
    memcpy(socket_address.sll_addr, dst_mac, 6);
    if (sendto(serverSocket, buffer_u.raw_data, sizeof(struct eth_hdr) + sizeof(struct ip_hdr) + sizeof(struct udp_hdr) + strlen(msg), 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0)
        printf("Send failed\n");
}
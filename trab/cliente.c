#include <arpa/inet.h>
#include <pthread.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
#include "raw.h"

#define PROTO_UDP	17
#define DST_PORT	8000


char this_mac[6];
char bcast_mac[6] =	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
char dst_mac[6] =	{0xa4, 0x1f, 0x72, 0xf5, 0x90, 0xb7};
char src_mac[6] =	{0xa4, 0x1f, 0x72, 0xf5, 0x90, 0xb7};

struct ifreq if_idx, if_mac, ifopts;
char ifName[IFNAMSIZ];
char *p;
struct sockaddr_in serverAddr;
struct sockaddr_ll socket_address;
int clientSocket, numbytes;
int continua = 1;

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

int main(int argc, char *argv[])
{
	struct ifreq if_idx, if_mac, ifopts;
	char ifName[IFNAMSIZ];
	char *p;
	struct sockaddr_in serverAddr;
	struct sockaddr_ll socket_address;
	int clientSocket, numbytes;
	uint8_t msg[100];
	uint8_t input[80];


	/* Get interface name */
	if (argc > 1)
		strcpy(ifName, argv[1]);
	else
		strcpy(ifName, DEFAULT_IF);

	/* Open RAW socket */
	if ((clientSocket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1)
		perror("socket");

	/* Set interface to promiscuous mode */
	strncpy(ifopts.ifr_name, ifName, IFNAMSIZ-1);
	ioctl(clientSocket, SIOCGIFFLAGS, &ifopts);
	ifopts.ifr_flags |= IFF_PROMISC;
	ioctl(clientSocket, SIOCSIFFLAGS, &ifopts);

	/* Get the index of the interface */
	memset(&if_idx, 0, sizeof(struct ifreq));
	strncpy(if_idx.ifr_name, ifName, IFNAMSIZ-1);
	if (ioctl(clientSocket, SIOCGIFINDEX, &if_idx) < 0)
		perror("SIOCGIFINDEX");
	socket_address.sll_ifindex = if_idx.ifr_ifindex;
	socket_address.sll_halen = ETH_ALEN;

	/* Get the MAC address of the interface */
	memset(&if_mac, 0, sizeof(struct ifreq));
	strncpy(if_mac.ifr_name, ifName, IFNAMSIZ-1);
	if (ioctl(clientSocket, SIOCGIFHWADDR, &if_mac) < 0)
		perror("SIOCGIFHWADDR");
	memcpy(this_mac, if_mac.ifr_hwaddr.sa_data, 6);

	/* End of configuration. Now we can send data using raw sockets. */

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8000);
	serverAddr.sin_addr.s_addr = inet_addr("10.0.2.15");
	/*if(connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0){ //TODO
		perror("connect Error ");
		return 0;
	}*/

	//printf("Conectado ao Servidor!!\n");

	/* Fill the Ethernet frame header */
	memcpy(buffer_u.cooked_data.ethernet.dst_addr, bcast_mac, 6);
	memcpy(buffer_u.cooked_data.ethernet.src_addr, src_mac, 6);
	buffer_u.cooked_data.ethernet.eth_type = htons(ETH_P_IP);
	
	char name[15];
	char aux[15];
	char canal[15];
	/*pthread_t thread;
	if(pthread_create(&thread, NULL, &receive, NULL)) {
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}	*/

	printf("Por favor comece inserindo um nick com o comando \\nick!\n\n");
	memset(canal, '\0', sizeof(canal));
	if(fork() == 0){
	while(continua){
		memset(aux, '\0', sizeof(aux));
		fgets (input, sizeof(input), stdin);
		input[strlen(input)-1] = '\0';

		if(strncmp(input,"/nick",5)==0){
			memset(name, '\0', sizeof(name));
			strncpy ( name, &input[6], strlen(input) - 6 );
			printf("Alterando nick para %s!\n", name);
			sprintf(msg, "/NICK %s", name);
		} else if(strncmp(input,"/create",7)==0) {
			strncpy ( aux, &input[8], strlen(input) - 8);
			if(strlen(aux) != 0) {
				printf("Criando canal #%s !!!\n", aux);
				sprintf(msg, "/CREATE #%s", aux);
			} else {
				printf("Insira o nome do canal\n");
			}
		} else if(strncmp(input,"/remove",7)==0) {
			strncpy ( aux, &input[8], strlen(input) - 8 );
			if(strlen(aux) != 0) {
				printf("Removendo canal #%s !!!\n", aux);
				sprintf(msg, "/REMOVE #%s", aux);
			} else {
				printf("Insira o nome do canal\n");
			}
		} else if(strncmp(input,"/list",5)==0) {
			sprintf(msg, "/LIST");
			printf("Listando canais!\n");
		} else if(strncmp(input,"/join",5)==0) {
			strncpy ( aux, &input[7], strlen(input) - 6 );
			if(strlen(aux) != 0) {
				sprintf(msg, "/JOIN #%s", aux);
				printf("Entrando no canal #%s !!!\n", aux);
				strcpy(canal, aux);
			} else {
				printf("Insira o nome do canal\n");
			}
		} else if(strncmp(input,"/part",5)==0) {
			sprintf(msg, "/PART #%s", aux);
			if(strlen(canal) == 0)
			{
				printf("Você não está em um servidor!\n");
				continue;
			}
			printf("Saindo do canal #%s!\n", canal);
			memset(canal, '\0', sizeof(canal));
		} else if(strncmp(input,"/names",6)==0) {
			sprintf(msg, "/NAMES #%s", canal);
			printf("Listando nomes!\n");
		} else if(strncmp(input,"/kick",5)==0) {
			strncpy ( aux, &input[6], strlen(input) - 6 );
			sprintf(msg, "/KICK #%s", aux);
			printf(">>KICK !!!\n"); //TODO
		} else if(strncmp(input,"/msg",4)==0) {
			strncpy ( aux, &input[5], strlen(input) - 5 );
			sprintf(msg, "/MSG %s", aux);
			printf(">>PRIVMSG !!!\n");
			//if(strlen(str2) != 0) {
			//	sprintf(echoString, "PRIVMSG %s %s", str2, str3);
			//}
		} else if(strncmp(input,"/quit",5)==0) {
			if(strlen(canal) == 0)
				sprintf(msg, "/QUIT");
			else
				sprintf(msg, "/QUIT #%s", canal);
			printf("Fechando o chat!\n");
			continua = 0;
		} else { //MSG
			sprintf(msg, "<%s> %s", name, input);
		}

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
		buffer_u.cooked_data.payload.ip.dst[0] = 10;
		buffer_u.cooked_data.payload.ip.dst[1] = 0;
		buffer_u.cooked_data.payload.ip.dst[2] = 2;
		buffer_u.cooked_data.payload.ip.dst[3] = 15;
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
		if (sendto(clientSocket, buffer_u.raw_data, sizeof(struct eth_hdr) + sizeof(struct ip_hdr) + sizeof(struct udp_hdr) + strlen(msg), 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0)
			printf("Send failed\n");

		printf("send\n");

		/*numbytes = recvfrom(clientSocket, buffer_u.raw_data, ETH_LEN, 0, NULL, NULL);
		if (buffer_u.cooked_data.ethernet.eth_type == ntohs(ETH_P_IP) ){

			if (buffer_u.cooked_data.payload.ip.proto == PROTO_UDP && buffer_u.cooked_data.payload.udp.udphdr.dst_port == ntohs(DST_PORT)){
				p = (char *)&buffer_u.cooked_data.payload.udp.udphdr + ntohs(buffer_u.cooked_data.payload.udp.udphdr.udp_len);
				*p = '\0';
				printf("> %s \n", (char *)&buffer_u.cooked_data.payload.udp.udphdr + sizeof(struct udp_hdr)); 
			}
		}*/

	}
	}
	else{
		while(continua){
		numbytes = recvfrom(clientSocket, buffer_u.raw_data, ETH_LEN, 0, NULL, NULL);
		if (buffer_u.cooked_data.ethernet.eth_type == ntohs(ETH_P_IP) ){

			if (buffer_u.cooked_data.payload.ip.proto == PROTO_UDP && buffer_u.cooked_data.payload.udp.udphdr.dst_port == ntohs(DST_PORT)){
				p = (char *)&buffer_u.cooked_data.payload.udp.udphdr + ntohs(buffer_u.cooked_data.payload.udp.udphdr.udp_len);
				*p = '\0';
				printf("%s \n", (char *)&buffer_u.cooked_data.payload.udp.udphdr + sizeof(struct udp_hdr)); 
			}
		}

	}
	}

	return 0;
}
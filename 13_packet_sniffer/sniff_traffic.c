#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

/*
 * Packet sniffer in C.
 *
 * References:
 *     1. https://www.gnu.org/software/libc/manual/html_node/Local-Socket-Example.html
 */

#define BUFF_SIZE 65536

int main() {

	// Create a raw socket listening for TCP packets
	printf("D> creating socket...\n");
	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
	if (sockfd < 0) {
		perror("E> Socket failed to open");
		return 1;
	}

	// Receive packets
	char buffer[BUFF_SIZE];
	int packet_count = 100;
	while (packet_count != 0) {
		ssize_t data_size = recvfrom(sockfd, buffer, BUFF_SIZE, 0, NULL, NULL);
		if ( data_size < 0 ) {
			perror("E> failed to receive data");
			continue;
		}
		printf("D> Captured packet with size %ld bytes\n", data_size);
		packet_count--;
	}

	// IP section
	struct iphdr *ip_header = (struct iphdr *)buffer;
	
	char src_ip[INET_ADDRSTRLEN], dest_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &ip_header->saddr, src_ip, INET_ADDRSTRLEN);
	inet_ntop(AF_INET, &ip_header->daddr, dest_ip, INET_ADDRSTRLEN);
	printf("D> S_IP %s, D_IP %s\n", src_ip, dest_ip);

	// TCP section
	struct tcphdr *tcp_header = (struct tcphdr *)(buffer + (ip_header->ihl*4));
	printf("D> S_P %u, D_P %u\n",
			ntohs(tcp_header->source), ntohs(tcp_header->dest));

	return 0;
}

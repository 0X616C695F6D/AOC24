#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <errno.h>

#define BUFF_SIZE 65536
#define MAX_CONNS 5

int conn_count = 0;
int running = 1;
struct Connection {
	char src_ip[INET_ADDRSTRLEN];
	char dest_ip[INET_ADDRSTRLEN];
	unsigned short src_port;
	unsigned short dest_port;
	char protocol[16];
	int packet_count;
};
struct Connection connections[MAX_CONNS];

/*
 * Check if connection already exists.
 *
 * Input:
 * src_ip -> source ip
 * src_port -> source port
 * dest_ip -> destination ip
 * dest_port -> destination port
 */
int does_exist(char *src_ip, char *dest_ip, unsigned short src_port,
		unsigned short dest_port) {
	// connection exists
	for (int i = 0; i < conn_count; i++) {
		if (strcmp(connections[i].src_ip, src_ip) == 0 &&
			strcmp(connections[i].dest_ip, dest_ip) == 0 &&
			connections[i].src_port == src_port &&
			connections[i].dest_port == dest_port) {
			return i;
		}
	}
	// connection does not exist
	return -1;
}

/*
 * Add a new connection to struct
 */
void add_conn(char *src_ip, char *dest_ip, unsigned short src_port,
		unsigned short dest_port, char *protocol) {
	// No more space for additional connections
	if (conn_count >= MAX_CONNS) {
		printf("D> Max connections");
		return;
	}
	// Add connection
	struct Connection *conn = &connections[conn_count++];
	strncpy(conn->src_ip, src_ip, INET_ADDRSTRLEN);
	strncpy(conn->dest_ip, dest_ip, INET_ADDRSTRLEN);
	conn->src_port = src_port;
	conn->dest_port = dest_port;
	strncpy(conn->protocol, protocol, sizeof(conn->protocol));
	conn->packet_count = 1;
}

// When an existing connection sends more packets
void update_packet_count(int index) {
	connections[index].packet_count++;
}

// Categorize destination port into HTTP, HTTPS or TCP
const char *get_protocol(unsigned short dest_port) {
	if (dest_port == 80) { return "HTTP"; }
	if (dest_port == 443) { return "HTTPS"; }
	return "TCP";
}

// Print connection information
void print_stats() {
	printf("\n===Connection Statistics===\n");
	for (int i = 0;  i < conn_count; i++) {
		printf("Connection %d\n", i + 1);
		printf("	Source %s:%u\n", connections[i].src_ip, connections[i].src_port);
		printf("	Dest %s:%u\n", connections[i].dest_ip, connections[i].dest_port);
		printf("	Protocol %s\n",connections[i].protocol); 
		printf("	Packet Count %d\n\n",connections[i].packet_count); 
	}
}

int main() {
	int sockfd;
	char buffer[BUFF_SIZE];

	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
	if (sockfd < 0) {
		printf("Failed to create a socket");
		return 1;
	}

	while (running && conn_count < MAX_CONNS) {
		struct sockaddr_in source;
		socklen_t addr_len = sizeof(source);

		ssize_t data_size = recvfrom(sockfd, buffer, BUFF_SIZE, 0,
				(struct sockaddr *)&source, &addr_len);
		if (data_size < 0) {
			perror("E> Failed to receive data");
			return 1;
		}

		struct iphdr *ip_header = (struct iphdr *)buffer;
		char src_ip[INET_ADDRSTRLEN], dest_ip[INET_ADDRSTRLEN];

		inet_ntop(AF_INET, &ip_header->saddr, src_ip, INET_ADDRSTRLEN);
		inet_ntop(AF_INET, &ip_header->saddr, src_ip, INET_ADDRSTRLEN);

		if (ip_header->protocol == IPPROTO_TCP) {
            struct tcphdr *tcp_header = (struct tcphdr *)(buffer + (ip_header->ihl * 4));
            unsigned short src_port = ntohs(tcp_header->source);
            unsigned short dest_port = ntohs(tcp_header->dest);

            const char *protocol = get_protocol(dest_port);

            int conn_index = does_exist(src_ip, dest_ip, src_port, dest_port);
            if (conn_index >= 0) {
                update_packet_count(conn_index);
            } else {
                add_conn(src_ip, dest_ip, src_port, dest_port, (char *)protocol);
            }
        }
	}

	print_stats();
	close(sockfd);

	return 0;
}

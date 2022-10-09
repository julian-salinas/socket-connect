#include "networking.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>

static int wait_client(int server_socket);
static int send_all(int socket_fd, void* buffer, size_t size);
static int recv_all(int socket_fd, void* buffer, size_t size);

int socket_create(char* ip, char* port, t_socket_type type) {
	struct addrinfo hints;
	struct addrinfo* server_info;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, port, &hints, &server_info);
	
	int socket_fd = socket(server_info -> ai_family, server_info -> ai_socktype, server_info -> ai_protocol);

	if (type == TYPE_SOCKET_SERVER) {
		bind(socket_fd, server_info -> ai_addr, server_info -> ai_addrlen);
		listen(socket_fd, SOMAXCONN);
		setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
	}
	else {
		connect(socket_fd, server_info -> ai_addr, server_info -> ai_addrlen);
	}

	freeaddrinfo(server_info);

	return socket_fd;
}

void socket_destroy(int socket_fd) {
	close(socket_fd);
}

void server_listen(int server_socket, void*(*handler)(void*), void* args) {
	printf("Server listening!\n");
	while (1) {
		int client_socket = wait_client(server_socket);

		if (client_socket == -1) {
			return;
		}

		t_server_args* server_args = malloc(sizeof(t_server_args));
		server_args -> socket_fd = client_socket;
		server_args -> data = args;

		pthread_t service_thread;
		pthread_create(&service_thread, NULL, handler, (void*) server_args);
		pthread_detach(service_thread);
	}
}

int socket_get(int socket_fd, void* dest, size_t size) {
    if(size != 0) {
        int rcv = recv_all(socket_fd, dest, size);
        
		if (rcv < 1) {
			return 0;
		}
    }
    return 1;
}

void socket_send(int socket_fd, void* src, size_t size) {
	send_all(socket_fd, src, size);
}

void send_package(int socket_fd, t_package* package) {
	socket_send(socket_fd, &package -> header, sizeof(uint8_t));
	socket_send(socket_fd, (void*) &package -> buffer -> offset, sizeof(uint32_t));
	socket_send(socket_fd, (void*) package -> buffer -> stream, package -> buffer -> offset);
}

t_package* recv_package(int socket_fd) {
	uint8_t header;

	// Receive header
	if (!socket_get(socket_fd, &header, sizeof(uint8_t))) {
		return NULL;
	}

	t_package* package = package_create(header);

	uint32_t buffer_size;
	
	// Receive buffer size
	if (!socket_get(socket_fd, &buffer_size, sizeof(uint32_t))) {
		package_destroy(package);
		return NULL;
	}

	package -> buffer = malloc(sizeof(t_buffer));
	package -> buffer -> offset = 0;
	package -> buffer -> size = buffer_size;
	package -> buffer -> stream = malloc(buffer_size);

	// Receive buffer
	if (!socket_get(socket_fd, package -> buffer -> stream, buffer_size)) {
		package_destroy(package);
		return NULL;
	}

	return package;
}

/***********************
 *  PRIVATE FUNCTIONS  *
 ***********************/

static int wait_client(int server_socket) {
	struct sockaddr_in client_dir;
	socklen_t addr_size = sizeof(struct sockaddr_in);
	int client_socket = accept(server_socket, (void*) &client_dir, &addr_size);
	return client_socket;
}

static int send_all(int socket_fd, void *buffer, size_t size) {
	while (size > 0) {
		int bytes_sent = send(socket_fd, buffer, size, 0);
		
		if (bytes_sent == 0) {
			return 0;
		}
			
		if (bytes_sent < 0) {
			return -1;
		}

		buffer += bytes_sent;
		size -= bytes_sent;
	}

	return 1;
}

static int recv_all(int socket, void *dest, size_t size) {
	while (size > 0){
		int bytes_received = recv(socket, dest, size, 0);
		
		if (bytes_received == 0) {
			return 0;
		} 
		
		if (bytes_received < 0) {
			return -1;
		} 

		dest += bytes_received;
		size -= bytes_received;
	}
	return 1;
}
#include "networking.h"
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>

static int send_all(int socket_fd, void* buffer, size_t size);
static int recv_all(int socket_fd, void* buffer, size_t size);
static int wait_client(int server_socket);
static int socket_get(int socket, void* dest, size_t size);

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
	while (1) {
		int client_socket = wait_client(server_socket);

		if (client_socket == -1) {
			return;
		}
		
		pthread_t service_thread;
		pthread_create(&service_thread, NULL, handler, (void*) args);
		pthread_detach(service_thread);
	}
}

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

void send_package(int socket_fd, t_package* package) {
	send_all(socket_fd, &package -> header, sizeof(uint8_t));
	send_all(socket_fd, &package -> buffer, package -> buffer -> size);
}

static int socket_get(int socket, void* dest, size_t size) {
    if(size != 0){
        int rc = recv_all(socket, dest, size);
        
		if(rc < 1) {
			return 0;
		}
    }
    return 1;
}

t_package* recv_package(int socket_fd) {
	uint8_t header;
	socket_get(socket_fd, &header, sizeof(uint8_t));

	size_t buffer_size;
	if (!socket_get(socket_fd, &buffer_size, sizeof(size_t))) {
		return NULL;
	}

	t_package *package = package_create(header);
	package -> buffer -> size = buffer_size;
	package -> buffer -> stream = malloc(buffer_size);

	if (!socket_get(socket_fd, package -> buffer -> stream, buffer_size)) {
		package_destroy(package);
		return NULL;
	}

	return package;
}
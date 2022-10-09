#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "connect/networking.h"
#include "connect/serialization.h"

void process_connection(int socket_fd, void* args);

int main(int argc, char** argv) {
	int socket_fd = socket_create("127.0.0.1", "8000", TYPE_SOCKET_SERVER);
	server_listen(socket_fd, process_connection, NULL);
	return 0;
}

void process_connection(int socket_fd, void* args) {

	printf("Client connected! SID: %d\n", socket_fd);

	t_package* package = recv_package(socket_fd);

	if (package -> header < 0) {
		printf("Client disconnected! SID: %d\n", socket_fd);
		return;
	}

	printf("Received package with header %d and buffer size %d\n", 
			package -> header, package -> buffer -> size);

	char* str = package_get_str(package);
	uint32_t* numero_random = (uint32_t*) package_get(package, sizeof(uint32_t));

	printf("Received string: %s\n", str);
	printf("Received number: %d\n", *numero_random);

	printf("Sending response...\n");
	t_package *response = package_create(5); // random operation code (5)
	package_add_str(response, "Hola, soy el server!");
	send_package(socket_fd, response);

	socket_destroy(socket_fd);
}
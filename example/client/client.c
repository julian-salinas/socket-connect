#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "connect/networking.h"
#include "connect/serialization.h"

int main(int argc, char** argv) {
	int socket_fd = socket_create("127.0.0.1", "8000", TYPE_SOCKET_CLIENT);

	t_package* package = package_create(5);
	
	char* str = "Hola que tal";
	uint32_t numero_random = 1234;

	package_add_str(package, str);
	package_add(package, &numero_random, sizeof(uint32_t));

	send_package(socket_fd, package);
	
	package_destroy(package);

	t_package* response = recv_package(socket_fd);
	
	if (response -> header < 0) {
		printf("Server closed connection! SID: %d\n", socket_fd);
		return 0;
	}

	if (response -> header > 0) {
		printf("Server sent a response - code: %d:\n", response -> header);
		char* response_text = package_get_str(response);
		printf("Response text: %s\n", response_text); 
		return 0;
	}

	socket_destroy(socket_fd);
	
	return 0;
}
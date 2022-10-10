#include <cspecs/cspec.h>
#include <connect/serialization.h>
#include <connect/networking.h>
#include <pthread.h>
#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>

sem_t server_ready;

int _wait_client(int server_socket) {
	struct sockaddr_in client_dir;
	socklen_t addr_size = sizeof(struct sockaddr_in);
	int client_socket = accept(server_socket, (void*) &client_dir, &addr_size);
	return client_socket;
}

/* Exactly the same function of server_listen, but not using an infinite loop */
void _server_listen(int server_socket, void(*handler)(int, void*), void* args) {
    int client_socket = _wait_client(server_socket);

    void _handler(void* server_args) {
        handler(client_socket, server_args);
    }

    if (client_socket == -1) {
        printf("Error accepting client!\n");
        socket_destroy(server_socket);
        return;
    }

    pthread_t service_thread;
    pthread_create(&service_thread, NULL, (void*) _handler, args);
    pthread_detach(service_thread);
}

void handler(int socket_fd, void* args) {
    t_package* package = recv_package(socket_fd);
    
    uint8_t header = package -> header;
    char* message = package_get_str(package);
    uint32_t* number = package_get(package, sizeof(uint32_t));

    t_package* response = package_create(header);
    package_add_str(response, message);
    package_add(response, number, sizeof(uint32_t));

    send_package(socket_fd, response);

    socket_destroy(socket_fd);
}

void test_server(void* args) {
    int server_socket = socket_create("127.0.0.1", "8000", TYPE_SOCKET_SERVER);
    _server_listen(server_socket, handler, NULL);
}

context (test_networking) {

	describe("Networking Tests") {
		it("Send a package a receive response") {
            sem_init(&server_ready, 0, 0);

            pthread_t server_thread;
            pthread_create(&server_thread, NULL, (void*) test_server, NULL);
            pthread_detach(server_thread);

            int client_socket_1 = socket_create("127.0.0.1", "8000", TYPE_SOCKET_CLIENT);
            t_package* package_1 = package_create(1);
            package_add_str(package_1, "Hola, soy el cliente y este es el primer mensaje!");
            package_add(package_1, &((uint32_t) {123456789}), sizeof(uint32_t));
            send_package(client_socket_1, package_1);

            t_package* response_1 = recv_package(client_socket_1);
            uint8_t header_1 = response_1 -> header;
            char* message_1 = package_get_str(response_1);
            int* number_1 = (uint32_t*) package_get(response_1, sizeof(uint32_t));

            should_int(header_1) be equal to(1);
            should_string(message_1) be equal to("Hola, soy el cliente y este es el primer mensaje!");
            should_int(*number_1) be equal to(123456789);
            
            socket_destroy(client_socket_1);

            int client_socket_2 = socket_create("127.0.0.1", "8000", TYPE_SOCKET_CLIENT);
            t_package* package_2 = package_create(3);
            send_package(client_socket_2, package_2);

            t_package* response_2 = recv_package(client_socket_2);
            uint8_t header_2 = response_2 -> header;

            should_int(header_2) be equal to(3);

            socket_destroy(client_socket_2);
        } end
    } end
}
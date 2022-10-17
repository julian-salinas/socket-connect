#include <cspecs/cspec.h>
#include <connect/serialization.h>
#include <connect/networking.h>
#include <semaphore.h>
#include <pthread.h>

sem_t server_ready, server_down;

typedef enum {
	TEXT,
	NUMBERS,
	OK,
	ERROR
} status_code;

void handler(int socket_fd, void* args) {
	t_package* request = recv_package(socket_fd);

	t_package* response = package_create(OK);
	switch (request -> header) {
		case TEXT:
			char* message = package_get_str(request);
			char* response_text = message;
			package_add_str(response, response_text);
			break;
		
		case NUMBERS:
			int* first_number = (int*) package_get(request, sizeof(int));
			int* second_number = (int*) package_get(request, sizeof(int));
			int result = *first_number + *second_number;
			package_add(response, &result, sizeof(int));
			break;
	}

	send_package(socket_fd, response);
	package_destroy(request);
	package_destroy(response);
	socket_destroy(socket_fd);
}

void* server(void* args) {
	int server_socket = socket_create("127.0.0.1", "8000", TYPE_SOCKET_SERVER);
	sem_post(&server_ready);
	server_listen(server_socket, handler, NULL);
	sem_wait(&server_down);
	pthread_cancel(server_socket);
	socket_destroy(server_socket);
	return NULL;
}

context(test_networking) {

	describe ("Networking Tests") {

		it ("create a server and a client") {
			sem_init(&server_ready, 0, 0);
			sem_init(&server_down, 0, 0);
			
			pthread_t server_thread;
			pthread_create(&server_thread, NULL, (void*) server, NULL);
			pthread_detach(server_thread);
			
			sem_wait(&server_ready);
			int socket_fd = socket_create("127.0.0.1", "8000", TYPE_SOCKET_CLIENT);

			should_int(socket_fd) not be equal to(-1);

			socket_destroy(socket_fd);
			sem_post(&server_down);
		} end

		it ("Send a text message to server") {
			sem_init(&server_ready, 0, 0);

			pthread_t server_thread;
			pthread_create(&server_thread, NULL, (void*) server, NULL);
			pthread_detach(server_thread);
			
			sem_wait(&server_ready);
			int socket_fd = socket_create("127.0.0.1", "8000", TYPE_SOCKET_CLIENT);

			t_package* request = package_create(TEXT);
			char* request_message = "I am not superstitious, but I am a little stitious.";
			package_add_str(request, request_message);
			send_package(socket_fd, request);
			package_destroy(request);

			t_package* response = recv_package(socket_fd);
			char* response_message = package_get_str(response);
			package_destroy(response);

			should_string(response_message) be equal to(request_message);

			free(response_message);
			socket_destroy(socket_fd);
			sem_post(&server_down);
		} end
		
		it ("Send numbers to the server and get the result of making the sum") {
			sem_init(&server_ready, 0, 0);

			pthread_t server_thread;
			pthread_create(&server_thread, NULL, (void*) server, NULL);
			pthread_detach(server_thread);
			
			sem_wait(&server_ready);
			int socket_fd = socket_create("127.0.0.1", "8000", TYPE_SOCKET_CLIENT);
			
			t_package* request_package = package_create(NUMBERS);
			int first_number = 5;
			int second_number = 10;
			package_add(request_package, &first_number, sizeof(int));
			package_add(request_package, &second_number, sizeof(int));
			send_package(socket_fd, request_package);
			package_destroy(request_package);

			t_package* response = recv_package(socket_fd);
			int* result = (int*) package_get(response, sizeof(int));
			package_destroy(response);

			should_int(*result) be equal to(first_number + second_number);
			
			free(result);

			socket_destroy(socket_fd);
			sem_post(&server_down);
		} end

	} end

}
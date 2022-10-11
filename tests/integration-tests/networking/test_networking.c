#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <connect/networking.h>

sem_t server_ready;

void handler(int socket_fd, void* args) {
    printf("Client connected!\n");
}

void server(void* args) {
    int server_socket = socket_create("127.0.0.1", "8000", TYPE_SOCKET_SERVER);
    sem_post(&server_ready);
    server_listen(server_socket, handler, NULL);
}

int main(int argc, char** argv) {
    sem_init(&server_ready, 0, 0);

    pthread_t server_thread;
    pthread_create(&server_thread, NULL, (void*) server, NULL);
    pthread_detach(server_thread);

    sem_wait(&server_ready);

    int client_socket = socket_create("127.0.0.1", "8000", TYPE_SOCKET_CLIENT);
    t_package* package = package_create(0);
    package_add_str(package, "Hello world!");
    send_package(client_socket, package);

    return 0;
}
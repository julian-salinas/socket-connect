#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../../src/networking.h"
#include "../../src/serialization.h"

void process_connection(void* args);

int main(int argc, char** argv) {
    int socket_fd = socket_create("127.0.0.1", "8000", TYPE_SOCKET_SERVER);
    server_listen(socket_fd, (void*) process_connection, NULL);
    return 0;
}

void process_connection(void* args) {
    t_server_args* server_args = (t_server_args*) args;
    int socket_fd = server_args -> socket_fd;

    printf("Client connected! SID: %d\n", socket_fd);

    t_package* package = recv_package(socket_fd);
    if (package == NULL) {
        printf("Connection closed by client\n");
        return;
    }

    printf("Received package with header %d and buffer size %d - offset: %d\n", 
            package -> header, package -> buffer -> size, package -> buffer -> offset);

    char* str = package_take_str(package);
    printf("Received string: %s\n", str);
}
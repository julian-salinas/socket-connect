#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../../src/networking.h"
#include "../../src/serialization.h"

void process_connection(void* args);

int main(int argc, char** argv) {
    int socket_fd = socket_create("127.0.0.1", "8000", TYPE_SOCKET_SERVER);
    server_listen(socket_fd, (void*) process_connection, NULL);
}

void process_connection(void* args) {
    int socket_fd = (int) ((t_server_args*) args) -> socket_fd;
    char* name = ((t_server_args*) args) -> data;

    t_package* package = recv_package(socket_fd);

    if (package == NULL) {
        printf("Connection closed by client\n");
        return;
    }

    printf("Received package with header %d and buffer size %d\n", package -> header, package -> buffer -> size);

    switch (package -> header) {
        case 0: // Mensaje
            int* message_size = package_take(package, sizeof(uint32_t));
            char* message = package_take(package, *message_size);
            printf("Received message: %s\n", message);
            free(message_size);
            free(message);
            break;
        
        case 1: // Fecha
            int* dia = package_take(package, sizeof(int32_t));
            int* mes = package_take(package, sizeof(int32_t));
            int* anio = package_take(package, sizeof(int32_t));

            printf("Received date: %d/%d/%d\n", *dia, *mes, *anio);

            free(dia);
            free(mes);
            free(anio);
            break;
    }
}
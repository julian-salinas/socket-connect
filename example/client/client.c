#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "../../src/networking.h"
#include "../../src/serialization.h"

int main(int argc, char** argv) {
    int socket_fd = socket_create("127.0.0.1", "8000", TYPE_SOCKET_CLIENT);

    t_package* package = package_create(5);
    
    char* str = "Hola que tal";
    uint32_t numero_random = 1234;

    package_add_str(package, "Hola mundo");
    package_add(package, &numero_random, sizeof(uint32_t));

    send_package(socket_fd, package);
    
    package_destroy(package);
    
    return 0;
}
#include <stdio.h>
#include <stdint.h>
#include "../../src/networking.h"
#include "../../src/serialization.h"

int main(int argc, char** argv) {
    int socket_fd = socket_create("127.0.0.1", "8000", TYPE_SOCKET_CLIENT);

    t_package* package = package_create(0);
    
    package_add_str(package, "Hola mundo");

    send_package(socket_fd, package);
        
    return 0;
}
#include <cspecs/cspec.h>
#include <connect/serialization.h>
#include <connect/networking.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>

sem_t server_started;

context (test_networking) {

	describe("Networking Tests") {
		it("communication between client and server") {
            sem_init(&server_started, 0, 0);

            void create_test_server(void* args) {
            int server_socket = socket_create("127.0.0.1", "8000", TYPE_SOCKET_SERVER);

            void handler(int socket_fd, void* args) {
                t_package* package = recv_package(socket_fd);
                
                uint8_t header = package -> header;
                
                if (header == 1) {
                    should_int(package -> buffer -> offset) be equal to(0);

                    char* str = package_get_str(package);
                    should_string(str) be equal to("Hola, soy el cliente y este es el primer mensaje!");

                    uint32_t* numero_random = (uint32_t*) package_get(package, sizeof(uint32_t));
                    should_int(*numero_random) be equal to(123456789);

                    should_int(package -> buffer -> offset) be equal to(strlen(str) + sizeof(uint32_t) + 1 + sizeof(uint32_t));
                    should_int(package -> buffer -> size) be equal to(package -> buffer -> offset);
                }
                else if (header == 2) {
                    should_int(package -> buffer -> offset) be equal to(0);

                    char* str = package_get_str(package);
                    should_string(str) be equal to("Hola, soy el cliente y este es el segundo mensaje!");

                    uint32_t* numero_random = (uint32_t*) package_get(package, sizeof(uint32_t));
                    should_int(*numero_random) be equal to(987654321);

                    should_int(package -> buffer -> size) be equal to(strlen(str) + sizeof(uint32_t) + 1 + sizeof(uint32_t));
                }
                else {
                    should_ptr(package  -> buffer -> stream) be equal to(NULL);
                    should_int(package  -> buffer -> size) be equal to(0);
                    should_int(package  -> buffer -> offset) be equal to(0);
                }
                socket_destroy(socket_fd);
            }

            int fork_result = fork();
            if (fork_result == 0) {
                server_listen(server_socket, handler, NULL);
            }
            
            else {
                sem_post(&server_started);
                wait(NULL);
            }
        }

            pthread_t server_thread;
            pthread_create(&server_thread, NULL, (void*) create_test_server, NULL);
            pthread_detach(server_thread);

            sem_wait(&server_started);
            int client_socket_1 = socket_create("127.0.0.1", "8000", TYPE_SOCKET_CLIENT);
            t_package* package_1 = package_create(1);
            package_add_str(package_1, "Hola, soy el cliente y este es el primer mensaje!");
            package_add(package_1, &((uint32_t) {123456789}), sizeof(uint32_t));
            send_package(client_socket_1, package_1);
            socket_destroy(client_socket_1);

            int client_socket_2 = socket_create("127.0.0.1", "8000", TYPE_SOCKET_CLIENT);
            t_package* package_2 = package_create(2);
            package_add_str(package_2, "Hola, soy el cliente y este es el segundo mensaje!");
            package_add(package_2, &((uint32_t) {987654321}), sizeof(uint32_t));
            send_package(client_socket_2, package_2);
            socket_destroy(client_socket_2);

            int client_socket_3 = socket_create("127.0.0.1", "8000", TYPE_SOCKET_CLIENT);
            t_package* package_3 = package_create(3);
            send_package(client_socket_3, package_3);
            socket_destroy(client_socket_3);

            // Force the server to close
            send_package(client_socket_1, package_1);
        } end
    } end
}
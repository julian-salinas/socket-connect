#ifndef NETWORKING_H
#define NETWORKING_H

	typedef enum {
		TYPE_SOCKET_SERVER,
		TYPE_SOCKET_CLIENT
	} t_socket_type;

	int socket_create(char* ip, char* port, t_socket_type type);
	
	void socket_destroy(int socket_fd);

	int wait_client(int server_socket);
	
	int server_listen(int server_socket, void*(*handler)(void*), void* args);


#endif /* NETWORKING_H */
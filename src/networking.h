#ifndef NETWORKING_H
#define NETWORKING_H

	#include "serialization.h"

	/**
	* @NAME: t_socket_type
	* @DESC: Tipo de socket.
	*/	
	typedef enum {
		TYPE_SOCKET_SERVER,
		TYPE_SOCKET_CLIENT
	} t_socket_type;

	/**
	* @NAME: t_server_args
	* @DESC: Argumentos para el hilo del servidor.
	*/
	typedef struct {
		int socket_fd;
		void* data;
	} t_server_args;

	/**
	* @NAME: socket_create
	* @DESC: Crea un socket de un tipo indicado.
	* @PARAMS:
	*   ip   - IP en la cual se conectará el socket, en caso de crear un socket de tipo cliente, 
    *          el socket se conectará a dicha IP, en caso de crear un socket de tipo servidor,
	*          el socket escuchará en dicha IP. Ejemplo: "127.0.0.1"
	*
	*   port - Ídem IP. Puerto al cual se conectará al socket cliente o puerto en el cual escuchará
	*          el socket servidor. Ejemplo: "8000"
	*
	* 	type - Tipo de socket a crear.
	*           TYPE_SOCKET_SERVER: Socket servidor.
	*           TYPE_SOCKET_CLIENT: Socket cliente.
	* @RETURN: Socket creado.
	*/
	int socket_create(char* ip, char* port, t_socket_type type);
	
	/**
	* @NAME: socket_destroy
	* @DESC: Destruye un socket (cierra la conexión).
	* @PARAMS:
	*   socket - Socket a destruir. (Es independiente al tipo del socket).
	*/
	void socket_destroy(int socket_fd);

	/**
	* @NAME: server_listen
	* @DESC: Escucha conexiones entrantes en un socket servidor.
	* @PARAMS:
	*   socket_fd - Socket servidor.
	*   handler   - Función a ejecutar cuando se reciba una conexión.
	*   args      - Argumentos a pasar a la función handler.
	*
	* El handler recibirá como argumento al cliente conectado, para poder sustraerlo, se debe
	* utilizar de la siguiente forma (ejemplo):
	*
	* void handler(void* args) {
	*	int socket_fd = (int) (t_server_args*) args -> socket;
	*	void* data = (t_server_args*) args -> data;
	*	free(args);
	*	// ...
	* }
	*/	
	void server_listen(int server_socket, void*(*handler)(void*), void* args);

	/**
	* @NAME: send_package
	* @DESC: Envia un paquete a un socket.
	* @PARAMS:
	*   socket_fd - Socket al cual enviar el paquete.
	*   package   - Paquete a enviar.
	*/
	void send_package(int socket_fd, t_package* package);

	/**
	* @NAME: recv_package
	* @DESC: Recibe un paquete de un socket.
	* @PARAMS:
	*   socket_fd - Socket del cual recibir el paquete.
	* @RETURN: Puntero al paquete recibido.
	*/
	t_package* recv_package(int socket_fd);

#endif /* NETWORKING_H */
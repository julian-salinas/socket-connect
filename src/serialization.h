#ifndef SERIALIZATION_H
#define SERIALIZATION_H

	#include <stdint.h>
	#include <string.h>

	/**
	* @NAME: t_buffer
	* @DESC: Buffer de datos que se envía dentro de un paquete.
	*/
	typedef struct {
		uint32_t size;
		uint32_t offset;
		void* stream;
	} t_buffer;

	/**
	* @NAME: t_package
	* @DESC: Paquete serializado.
	* @PARAMS:
	*   header - Header del paquete (código de operación).
	*   buffer - Buffer de datos.
	*/	
	typedef struct {
		uint8_t header;
		t_buffer* buffer;
	} t_package;

	/**
	* @NAME: package_create
	* @DESC: Crea un paquete.
	* @PARAMS:
	*   header - Header del paquete (código de operación).
	*/
	t_package* package_create(uint8_t header);

	/**
	* @NAME: package_destroy
	* @DESC: Destruye un paquete (libera memoria ocupada por el paquete y su respectivo buffer).
	* @PARAMS:
	*   package - Puntero al paquete a destruir.
	*/
	void package_destroy(t_package* package);

	/**
	* @NAME: package_append
	* @DESC: Agrega datos al buffer de un paquete.
	* @PARAMS:
	*   package - Puntero al paquete al cual agregar datos.
	*   data    - Dato/s a agregar.
	*   size    - Tamaño de los datos a agregar.
	*/
	void package_append(t_package* package, void* data, size_t size);

	/**
	* @NAME: package_take	
	* @DESC: Obtiene un valor de un paquete (deserializa).
	* @PARAMS:
	*   package - Puntero al paquete del cual obtener datos.
	*   size    - Tamaño del dato que se quiere obtener.
	*/
	void* package_take(t_package* package, size_t size);

#endif /* SERIALIZATION_H */
#ifndef SERIALIZATION_H
#define SERIALIZATION_H

	#include <stdint.h>

	typedef struct {
		uint32_t size;
		uint32_t offset;
		void* stream;
	} t_buffer;

	typedef struct {
		uint8_t header;
		t_buffer* buffer;
	} t_package;

	t_package* package_create(uint8_t header);

	void package_destroy(t_package* package);

	void package_append(t_package* package, void* data, size_t size);

	void* package_take(t_package* package, size_t size);

#endif /* SERIALIZATION_H */
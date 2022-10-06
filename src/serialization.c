#include "serialization.h"
#include <stdlib.h>
#include <stdio.h> // sacar

t_buffer* buffer_create(void);
void buffer_destroy(t_buffer* buffer);
static void buffer_add(t_buffer* buffer, void* data, size_t size);
static void fix_buffer_size(t_buffer* buffer, size_t size);
static void* buffer_take(t_buffer* buffer, size_t size);

t_package* package_create(uint8_t header) {
	t_package* self = malloc(sizeof(t_package));
	self -> header = header;
	self -> buffer = buffer_create();
	return self;
}

void package_destroy(t_package* package) {
	buffer_destroy(package -> buffer);
	free(package);
}

void package_add(t_package* package, void* data, size_t size) {
	buffer_add(package -> buffer, data, size);
}

void* package_take(t_package* package, size_t size) {
	return buffer_take(package -> buffer, size);
}

void package_add_str(t_package* package, char* str) {
	uint32_t string_size = strlen(str) + 1;
	package_add(package, &string_size, sizeof(uint32_t));
	package_add(package, (void*) str, string_size);
}

char* package_take_str(t_package* package) {
	uint32_t* string_size = (uint32_t*) package_take(package, sizeof(uint32_t));
	char* str = (char*) package_take(package, *string_size);
	free(string_size);
	return str;
}

/***********************
 *  PRIVATE FUNCTIONS  *
 ***********************/

t_buffer* buffer_create(void) {
	t_buffer* self = malloc(sizeof(t_buffer));
	self -> size = 0;
	self -> offset = 0;
	self -> stream = NULL;
	return self;
}

void buffer_destroy(t_buffer* buffer) {
	if (buffer -> offset > 0) {
		free(buffer -> stream);
	}
	free(buffer);
}

static void buffer_add(t_buffer* buffer, void* data, size_t size) {
	fix_buffer_size(buffer, size);
	memcpy(buffer -> stream + buffer -> offset, data, size);
	buffer -> offset += size;
}

static void fix_buffer_size(t_buffer* buffer, size_t size) {
	if (buffer -> size <= buffer -> offset + size) {
		buffer -> size += size;
		buffer -> stream = realloc(buffer -> stream, buffer -> size);
	}
}

static void _buffer_take(t_buffer* buffer, void** dest, size_t size) {
	*dest = calloc(1, size);
    memcpy(*dest, buffer -> stream + buffer->offset, size);
    buffer -> offset += size;
}

static void* buffer_take(t_buffer* buffer, size_t size) {
	void* data = NULL;
	_buffer_take(buffer, &data, size);
	return data;
}
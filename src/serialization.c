#include "serialization.h"
#include <stdlib.h>

static t_buffer* buffer_create(void);
static void buffer_destroy(t_buffer* buffer);
static void buffer_append(t_buffer* buffer, void* data, size_t size);
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

void package_append(t_package* package, void* data, size_t size) {
	buffer_append(package -> buffer, data, size);
}

void* package_take(t_package* package, size_t size) {
	return buffer_take(package -> buffer, size);
}

/***********************
 *  PRIVATE FUNCTIONS  *
 ***********************/

static t_buffer* buffer_create(void) {
	t_buffer* self = malloc(sizeof(t_buffer));
	self -> size = 0;
	self -> offset = 0;
	self -> stream = NULL;
	return self;
}

static void buffer_destroy(t_buffer* buffer) {
	free(buffer -> stream);
	free(buffer);
}

static void buffer_append(t_buffer* buffer, void* data, size_t size) {
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

static void* buffer_take(t_buffer* buffer, size_t size) {
	void* data = malloc(size);
	memcpy(data, buffer -> stream + buffer -> offset, size);
	buffer->offset += size;
	return data;
}
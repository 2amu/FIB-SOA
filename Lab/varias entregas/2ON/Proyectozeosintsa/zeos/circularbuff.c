#include <circularbuff.h>


// Función para inicializar el buffer circular
void initCircularBuffer(struct CircularBuffer *buffer) {
    buffer->head = 0;
    buffer->tail = 0;
}

// Función para añadir un elemento al buffer circular
void addToCircularBuffer(struct CircularBuffer *buffer, char value) {
    buffer->vec[buffer->head] = value;
    buffer->head = (buffer->head + 1) % CIRCULARBUFF_SIZE;
}

// Función para leer un elemento del buffer circular
char readFromCircularBuffer(struct CircularBuffer *buffer) {
    char value = buffer->vec[buffer->tail];
    buffer->tail = (buffer->tail + 1) % CIRCULARBUFF_SIZE;
    return value;
}
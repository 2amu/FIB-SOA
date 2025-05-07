#define CIRCULARBUFF_SIZE 5

struct CircularBuffer {
    char vec[CIRCULARBUFF_SIZE];
    int head;  // Índice para escribir datos
    int tail;  // Índice para leer datos
};

// Función para inicializar el buffer circular
void initCircularBuffer(struct CircularBuffer *buffer);

// Función para añadir un elemento al buffer circular
void addToCircularBuffer(struct CircularBuffer *buffer, char value);

// Función para leer un elemento del buffer circular
char readFromCircularBuffer(struct CircularBuffer *buffer);


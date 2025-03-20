#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h> // Para inet_ntoa y ntohs

#define PORT 8080
#define MAX_CLIENTS 10
#define THREAD_POOL_SIZE 4
#define MAX_TASKS 20
#define BUFFER_SIZE 1024
#define MAX_KEY_LENGTH 64
#define MAX_VALUE_LENGTH 256

// (Incluir aquí las definiciones de task_t y thread_pool_t del Bloque 10)
typedef struct {
    void (*function)(void *);
    void *argument;
} task_t;

typedef struct {
    task_t *tasks[1][MAX_TASKS];
    int head[1];
    int tail[1];
    int count[1];
    int capacity;
    pthread_mutex_t queue_mutex;
    pthread_cond_t queue_not_empty[1];
    pthread_cond_t queue_not_full[1];
    pthread_t threads[THREAD_POOL_SIZE];
    int shutdown;
} thread_pool_t;

void thread_pool_init(thread_pool_t *pool, int num_threads, int max_tasks);
void thread_pool_submit(thread_pool_t *pool, void (*function)(void *), void *argument);
void thread_pool_destroy(thread_pool_t *pool);
void *worker(void *pool);

// Estructura para el almacén clave-valor
typedef struct {
    char key[MAX_KEY_LENGTH];
    char value[MAX_VALUE_LENGTH];
} kv_entry_t;

typedef struct {
    kv_entry_t *store;
    int capacity;
    int size;
    pthread_rwlock_t rwlock;
} key_value_store_t;

key_value_store_t *kv_store_create(int capacity);
char *kv_store_get(key_value_store_t *store, const char *key);
int kv_store_put(key_value_store_t *store, const char *key, const char *value);
int kv_store_delete(key_value_store_t *store, const char *key);
void kv_store_destroy(key_value_store_t *store);

// Estructura para pasar información del cliente y el almacén a la tarea del thread pool
typedef struct {
    int client_fd;
    key_value_store_t *store;
} client_context_t;

void handle_client(void *arg);

// Implementaciones de thread pool (copia del Bloque 10)
void thread_pool_init(thread_pool_t *pool, int num_threads, int max_tasks) {
/* ... */
}
void thread_pool_submit(thread_pool_t *pool, void (*function)(void *), void *argument) {
  /* ... */
}
void *worker(void *pool) {
  /* ... */
}
void thread_pool_destroy(thread_pool_t *pool) {
  /* ... */
}

// Implementaciones del almacén clave-valor
key_value_store_t *kv_store_create(int capacity) {
    /*
    Crea e inicializa el almacén clave-valor concurrente.

    - Asigna memoria para la estructura del almacén.
    - Asigna memoria para el array de entradas clave-valor.
    - Inicializa el read-write lock para controlar el acceso concurrente.
    */
}

char *kv_store_get(key_value_store_t *store, const char *key) {
    /*
    Obtiene el valor asociado a una clave del almacén de forma concurrente para lectores.

    - Adquiere el read lock.
    - Itera a través del almacén para buscar la clave.
    - Si se encuentra, libera el lock y retorna el valor.
    - Si no se encuentra, libera el lock y retorna NULL.
    */
}

int kv_store_put(key_value_store_t *store, const char *key, const char *value) {
    /*
    Inserta o actualiza un par clave-valor en el almacén con escritura exclusiva.

    - Adquiere el write lock.
    - Busca si la clave ya existe para actualizar el valor.
    - Si no existe y hay espacio, añade una nueva entrada.
    - Libera el lock y retorna 0 en éxito, -1 si no hay espacio.
    */
}

int kv_store_delete(key_value_store_t *store, const char *key) {
    /*
    Elimina un par clave-valor del almacén con escritura exclusiva.

    - Adquiere el write lock.
    - Busca la clave y, si se encuentra, la elimina moviendo las entradas restantes.
    - Libera el lock y retorna 0 en éxito, -1 si no se encuentra.
    */
}

void handle_client(void *arg) {
    /*
    Maneja las peticiones de un cliente en un hilo del thread pool.

    - Recibe el descriptor del socket del cliente y el almacén clave-valor.
    - Lee comandos del cliente (GET, PUT, DELETE).
    - Parsea el comando y la clave (y el valor para PUT).
    - Realiza la operación correspondiente en el almacén clave-valor.
    - Envía una respuesta al cliente.
    - Cierra la conexión.
    */
}

int main() {
    int server_fd, new_socket, max_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    fd_set readfds;
    thread_pool_t pool;
    key_value_store_t *store;

    // Inicializar el thread pool
    thread_pool_init(&pool, THREAD_POOL_SIZE, MAX_TASKS);

    // Crear el almacén clave-valor
    store = kv_store_create(100); // Capacidad para 100 entradas
    if (!store) {
        perror("kv_store_create failed");
        exit(EXIT_FAILURE);
    }

    // Crear socket del servidor (igual que en el Bloque 10)
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { /* ... */ }
    int flags = fcntl(server_fd, F_GETFL, 0);
    if (fcntl(server_fd, F_SETFL, flags | O_NONBLOCK) < 0) { /* ... */ }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) { /* ... */ }
    if (listen(server_fd, MAX_CLIENTS) < 0) { /* ... */ }
    printf("Servidor escuchando en el puerto %d...\n", PORT);

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_fd = server_fd;

        int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) { /* ... */ }

        if (FD_ISSET(server_fd, &readfds)) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) { /* ... */ }
            printf("Nueva conexión aceptada, socket fd es %d, IP es: %s, puerto: %d\n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
            flags = fcntl(new_socket, F_GETFL, 0);
            if (fcntl(new_socket, F_SETFL, flags | O_NONBLOCK) < 0) { /* ... */ }

            client_context_t *context = malloc(sizeof(client_context_t));
            if (!context) { /* ... */ }
            context->client_fd = new_socket;
            context->store = store;
            thread_pool_submit(&pool, handle_client, context);
        }
    }

    close(server_fd);
    thread_pool_destroy(&pool);
    kv_store_destroy(store);
    return 0;
}

Compila: gcc pthreads11.c -o concurrent_kv_store -lpthread
Ejecuta: ./concurrent_kv_store
Explicación:
    -Almacén Clave-Valor Concurrente:
        Se implementa una estructura key_value_store_t
        que utiliza un array para almacenar las entradas clave-valor
        y un pthread_rwlock_t para controlar el acceso concurrente.
        Esto permite que múltiples lectores accedan al almacén simultáneamente,
        pero requiere un lock exclusivo para las operaciones de escritura (PUT y DELETE).

    -Operaciones del Almacén:
        Se proporcionan funciones para crear, obtener, insertar/actualizar
        y eliminar pares clave-valor de forma thread-safe.

    -Integración con el Servidor No Bloqueante y el Thread Pool:
        Al igual que en el Bloque 10,
        se utiliza un servidor no bloqueante con select para aceptar conexiones.
        Cada conexión aceptada se pasa como una tarea al thread pool.

    -handle_client con Almacén:
        La función handle_client ahora recibe un client_context_t
        que contiene tanto el descriptor del socket del cliente como un puntero
        al almacén clave-valor compartido.

    -Protocolo Simple:
        El cliente puede enviar comandos simples como GET <key>,
        PUT <key> <value>, y DELETE <key>.
        El servidor responde con OK, VALUE <value>, NOT_FOUND, o ERROR.

Para probar este servidor:

        Ejecuta el programa concurrent_kv_store.
            Puedes usar la herramienta netcat (nc) desde otra terminal
            para interactuar con el servidor.
    Por ejemplo:
        Para insertar un valor: echo "PUT mykey myvalue" | nc localhost 8080
        Para obtener un valor: echo "GET mykey" | nc localhost 8080
        Para eliminar una clave: echo "DELETE mykey" | nc localhost 8080
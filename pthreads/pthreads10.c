#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define THREAD_POOL_SIZE 4
#define MAX_TASKS 20
#define BUFFER_SIZE 1024

// (Incluir aquí las definiciones de task_t y thread_pool_t del Bloque 9, simplificadas si es necesario)
typedef struct {
    void (*function)(void *);
    void *argument;
    int priority;
} task_t;

typedef struct {
    task_t *tasks[1][MAX_TASKS]; // Simplificando a una única prioridad para este ejemplo
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
void thread_pool_submit(thread_pool_t *pool, void (*function)(void *), void *argument); // Simplificado sin prioridad
void thread_pool_destroy(thread_pool_t *pool);
void *worker(void *pool);

// Estructura para pasar información del cliente a la tarea del thread pool
typedef struct {
    int client_fd;
} client_info_t;

void handle_client(void *arg);

void thread_pool_init(thread_pool_t *pool, int num_threads, int max_tasks) {
    pool->capacity = max_tasks;
    for (int i = 0; i < 1; ++i) {
        pool->head[i] = pool->tail[i] = pool->count[i] = 0;
        pool->tasks[i] = malloc(sizeof(task_t) * pool->capacity);
        if (!pool->tasks[i]) perror("malloc tasks failed");
        pthread_cond_init(&pool->queue_not_empty[i], NULL);
        pthread_cond_init(&pool->queue_not_full[i], NULL);
    }
    pthread_mutex_init(&pool->queue_mutex, NULL);
    pool->shutdown = 0;
    for (int i = 0; i < num_threads; ++i) {
        pthread_create(&pool->threads[i], NULL, worker, pool);
    }
}

void thread_pool_submit(thread_pool_t *pool, void (*function)(void *), void *argument) {
    pthread_mutex_lock(&pool->queue_mutex);
    while (pool->count[0] == pool->capacity && !pool->shutdown) {
        pthread_cond_wait(&pool->queue_not_full[0], &pool->queue_mutex);
    }
    if (pool->shutdown) {
        pthread_mutex_unlock(&pool->queue_mutex);
        return;
    }
    pool->tasks[0][pool->tail[0]].function = function;
    pool->tasks[0][pool->tail[0]].argument = argument;
    pool->tail[0] = (pool->tail[0] + 1) % pool->capacity;
    pool->count[0]++;
    pthread_cond_signal(&pool->queue_not_empty[0]);
    pthread_mutex_unlock(&pool->queue_mutex);
}

void *worker(void *pool) {
    thread_pool_t *p = (thread_pool_t *)pool;
    while (1) {
        pthread_mutex_lock(&p->queue_mutex);
        while (p->count[0] == 0 && !p->shutdown) {
            pthread_cond_wait(&p->queue_not_empty[0], &p->queue_mutex);
        }
        if (p->shutdown) {
            pthread_mutex_unlock(&p->queue_mutex);
            pthread_exit(NULL);
        }
        task_t task = p->tasks[0][p->head[0]];
        p->head[0] = (p->head[0] + 1) % p->capacity;
        p->count[0]--;
        pthread_cond_signal(&p->queue_not_full[0]);
        pthread_mutex_unlock(&p->queue_mutex);
        task.function(task.argument);
    }
    return NULL;
}

void thread_pool_destroy(thread_pool_t *pool) {
    pthread_mutex_lock(&pool->queue_mutex);
    pool->shutdown = 1;
    pthread_cond_broadcast(&pool->queue_not_empty[0]);
    pthread_mutex_unlock(&pool->queue_mutex);
    for (int i = 0; i < THREAD_POOL_SIZE; ++i) {
        pthread_join(pool->threads[i], NULL);
    }
    for (int i = 0; i < 1; ++i) {
        free(pool->tasks[i]);
        pthread_cond_destroy(&pool->queue_not_empty[i]);
        pthread_cond_destroy(&pool->queue_not_full[i]);
    }
    pthread_mutex_destroy(&pool->queue_mutex);
}

void handle_client(void *arg) {
    /*
    Función que se ejecuta en un hilo del pool para manejar la comunicación con un cliente.

    - Recibe un puntero a una estructura client_info_t que contiene el descriptor del socket del cliente.
    - Lee datos del socket del cliente (no bloqueante).
    - Procesa los datos recibidos.
    - Envía una respuesta al cliente (no bloqueante).
    - Cierra la conexión con el cliente.
    */
}

int main() {
    int server_fd, new_socket, max_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    fd_set readfds;
    thread_pool_t pool;

    // Inicializar el thread pool
    thread_pool_init(&pool, THREAD_POOL_SIZE, MAX_TASKS);

    // Crear socket del servidor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Configurar socket no bloqueante
    int flags = fcntl(server_fd, F_GETFL, 0);
    if (fcntl(server_fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("fcntl nonblock failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind del socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Escuchar por conexiones
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Servidor escuchando en el puerto %d...\n", PORT);

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_fd = server_fd;

        // No estamos gestionando activamente los sockets de los clientes en el bucle principal
        // ya que el I/O se delega al thread pool.

        // Usar select para esperar por actividad en el socket del servidor (nuevas conexiones)
        int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            perror("select error");
            continue;
        }

        if (FD_ISSET(server_fd, &readfds)) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
                perror("accept failed");
                continue;
            }

            printf("Nueva conexión aceptada, socket fd es %d, IP es: %s, puerto: %d\n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            // Configurar el socket del cliente como no bloqueante (opcional, depende de la estrategia)
            flags = fcntl(new_socket, F_GETFL, 0);
            if (fcntl(new_socket, F_SETFL, flags | O_NONBLOCK) < 0) {
                perror("fcntl nonblock client failed");
                close(new_socket);
                continue;
            }

            // Crear una tarea para manejar al cliente y enviarla al thread pool
            client_info_t *client_info = malloc(sizeof(client_info_t));
            if (!client_info) {
                perror("malloc client_info failed");
                close(new_socket);
                continue;
            }
            client_info->client_fd = new_socket;
            thread_pool_submit(&pool, handle_client, client_info);
        }
    }

    // En un servidor real, aquí habría lógica para una terminación más controlada
    close(server_fd);
    thread_pool_destroy(&pool);
    return 0;
}

/*
Compila: gcc pthreads10.c -o nonblocking_io_pool -lpthread
Ejecuta: ./nonblocking_io_pool
Explicación:
    -Socket No Bloqueante:
        El socket del servidor se configura como no bloqueante
        usando fcntl(server_fd, F_SETFL, flags | O_NONBLOCK).
        Esto significa que las llamadas a accept() retornarán inmediatamente,
        incluso si no hay conexiones pendientes
        (en cuyo caso retornarán con un error EAGAIN o EWOULDBLOCK).

    -select para Multiplexación de I/O:
        La función select() se utiliza para esperar
        a que haya actividad en el socket del servidor
        (es decir, una nueva conexión entrante).
        select() permite al servidor esperar en múltiples descriptores de archivo a la vez,
        aunque en este ejemplo solo estamos esperando en el socket del servidor en el bucle principal.

    -Thread Pool para Manejo de Clientes:
        Cuando se acepta una nueva conexión, no se crea un nuevo hilo directamente para manejarla.
        En lugar de eso, se crea1 una estructura client_info_t
        que contiene el descriptor del socket del cliente,
        y esta estructura se pasa como argumento a la función handle_client.
        Esta función handle_client se envía como una tarea al thread pool.

    -handle_client en el Thread Pool:
        La función handle_client se ejecuta en uno de los hilos trabajadores del pool.
        Esta función es responsable de leer y escribir datos en el socket del cliente.
        En este ejemplo simplificado, se usa recv() y send(),
        que en sockets no bloqueantes también deberían manejarse de forma no bloqueante
        en un entorno real más complejo (posiblemente usando select o poll dentro del hilo trabajador
        si es necesario manejar múltiples operaciones de I/O para un solo cliente).

    -Beneficios:
        Este enfoque permite que un número limitado de hilos en el thread pool
        maneje potencialmente muchas conexiones de clientes.
        El bucle principal del servidor no se bloquea esperando a que los clientes envíen datos;
        simplemente acepta nuevas conexiones y las delega al pool de hilos.

Nota Importante:
Este es un ejemplo simplificado.
Un servidor real con I/O no bloqueante y un thread pool requeriría
un manejo mucho más sofisticado de los estados de conexión,
la lectura y escritura parcial de datos,
y posiblemente el uso de mecanismos de notificación de eventos más avanzados
(como epoll en Linux para un mayor rendimiento con un gran número de conexiones).
Sin embargo, este bloque sienta las bases para entender cómo integrar la I/O no bloqueante
con un thread pool usando select para la aceptación de conexiones.
 */
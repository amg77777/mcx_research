#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define INITIAL_THREADS 2
#define MAX_THREADS 5
#define MAX_TASKS 20

typedef struct {
    void (*function)(void *);
    void *argument;
} task_t;

typedef struct {
    task_t *tasks;
    int head;
    int tail;
    int count;
    int capacity;
    pthread_mutex_t queue_mutex;
    pthread_cond_t queue_not_empty;
    pthread_cond_t queue_not_full;

    pthread_t *threads;
    int num_threads;
    int max_threads;
    pthread_mutex_t pool_mutex; // Mutex para controlar el número de hilos
} thread_pool_t;

void thread_pool_init(thread_pool_t *pool, int initial_threads, int max_threads, int max_tasks);
void thread_pool_submit(thread_pool_t *pool, void (*function)(void *), void *argument);
void thread_pool_destroy(thread_pool_t *pool);
void *worker(void *pool);
int add_worker(thread_pool_t *pool);

void execute_task(void *arg) {
    int task_id = *(int *)arg;
    printf("Hilo %lu ejecutando tarea %d\n", pthread_self(), task_id);
    sleep(rand() % 5); // Simular trabajo más largo
    free(arg);
}

void thread_pool_init(thread_pool_t *pool, int initial_threads, int max_threads, int max_tasks) {
    /*
    Inicializa la estructura del thread pool con soporte para redimensionamiento dinámico.

    - Asigna memoria para la cola de tareas y los hilos.
    - Inicializa los mutexes para la cola y el pool de hilos.
    - Inicializa las variables de condición para la cola.
    - Establece la capacidad máxima de la cola y el número máximo de hilos.
    - Crea el número inicial de hilos trabajadores y los inicia.
    */
    pool->capacity = max_tasks;
    pool->tasks = malloc(sizeof(task_t) * pool->capacity);
    if (!pool->tasks) perror("malloc tasks failed");
    pool->head = pool->tail = pool->count = 0;
    pthread_mutex_init(&pool->queue_mutex, NULL);
    pthread_cond_init(&pool->queue_not_empty, NULL);
    pthread_cond_init(&pool->queue_not_full, NULL);

    pool->max_threads = max_threads;
    pool->num_threads = 0;
    pool->threads = malloc(sizeof(pthread_t) * pool->max_threads);
    if (!pool->threads) perror("malloc threads failed");
    pthread_mutex_init(&pool->pool_mutex, NULL);

    for (int i = 0; i < initial_threads; ++i) {
        if (add_worker(pool) != 0) {
            fprintf(stderr, "Error al inicializar los hilos iniciales\n");
            // Aquí se debería implementar una limpieza más robusta
        }
    }
}

void thread_pool_submit(thread_pool_t *pool, void (*function)(void *), void *argument) {
    /*
    Añade una tarea a la cola del thread pool y gestiona el redimensionamiento dinámico.

    - Bloquea el mutex de la cola.
    - Espera si la cola está llena.
    - Añade la tarea a la cola.
    - Incrementa el contador de tareas.
    - Señala que la cola ya no está vacía.
    - Comprueba si la cola está llena y si el número actual de hilos es menor que el máximo.
      Si ambas condiciones son verdaderas, intenta añadir un nuevo hilo trabajador.
    - Desbloquea el mutex de la cola.
    */
    pthread_mutex_lock(&pool->queue_mutex);
    while (pool->count == pool->capacity) {
        pthread_cond_wait(&pool->queue_not_full, &pool->queue_mutex);
    }
    pool->tasks[pool->tail].function = function;
    pool->tasks[pool->tail].argument = argument;
    pool->tail = (pool->tail + 1) % pool->capacity;
    pool->count++;
    pthread_cond_signal(&pool->queue_not_empty);

    if (pool->count == pool->capacity && pool->num_threads < pool->max_threads) {
        pthread_mutex_lock(&pool->pool_mutex);
        if (pool->num_threads < pool->max_threads) {
            printf("Redimensionando pool: añadiendo un nuevo hilo (actualmente %d)\n", pool->num_threads);
            add_worker(pool);
        }
        pthread_mutex_unlock(&pool->pool_mutex);
    }
    pthread_mutex_unlock(&pool->queue_mutex);
}

int add_worker(thread_pool_t *pool) {
    /*
    Añade un nuevo hilo trabajador al pool.

    - Bloquea el mutex del pool para modificar el número de hilos.
    - Crea un nuevo hilo que ejecuta la función 'worker'.
    - Incrementa el contador de hilos.
    - Desbloquea el mutex del pool.
    - Retorna 0 en éxito, -1 en error.
    */
    pthread_mutex_lock(&pool->pool_mutex);
    if (pool->num_threads < pool->max_threads) {
        if (pthread_create(&pool->threads[pool->num_threads], NULL, worker, pool) == 0) {
            pool->num_threads++;
            pthread_mutex_unlock(&pool->pool_mutex);
            return 0;
        } else {
            perror("Error al crear un nuevo hilo trabajador");
            pthread_mutex_unlock(&pool->pool_mutex);
            return -1;
        }
    }
    pthread_mutex_unlock(&pool->pool_mutex);
    return -1; // No se pueden añadir más hilos
}

void *worker(void *pool) {
    /*
    Función que ejecuta cada hilo trabajador del pool.

    - Entra en un bucle infinito para procesar tareas.
    - Bloquea el mutex de la cola.
    - Espera hasta que haya una tarea disponible o se indique el cierre.
    - Obtiene una tarea de la cola.
    - Desbloquea el mutex de la cola.
    - Ejecuta la tarea.
    */
    thread_pool_t *p = (thread_pool_t *)pool;
    while (1) {
        pthread_mutex_lock(&p->queue_mutex);
        while (p->count == 0) {
            pthread_cond_wait(&p->queue_not_empty, &p->queue_mutex);
        }
        task_t task = p->tasks[p->head];
        p->head = (p->head + 1) % p->capacity;
        p->count--;
        pthread_cond_signal(&p->queue_not_full);
        pthread_mutex_unlock(&p->queue_mutex);
        task.function(task.argument);
    }
    return NULL;
}

void thread_pool_destroy(thread_pool_t *pool) {
    /*
    Destruye el thread pool.

    - Bloquea el mutex de la cola.
    - Señala a todos los hilos que deben terminar (esto requeriría un mecanismo más sofisticado
      para una terminación limpia, como una bandera 'shutdown' y una condición).
    - Desbloquea el mutex de la cola.
    - Espera a que todos los hilos terminen.
    - Libera la memoria asignada.
    - Destruye los mutexes y las condiciones.
    */
    pthread_mutex_lock(&pool->queue_mutex);
    // En una implementación real, se necesitaría una forma de indicar a los hilos que terminen
    // de forma más elegante (ej. estableciendo pool->shutdown = 1 y señalando la condición).
    pthread_cond_broadcast(&pool->queue_not_empty); // Despertar a los hilos para que comprueben la condición de cierre
    pthread_mutex_unlock(&pool->queue_mutex);

    for (int i = 0; i < pool->num_threads; ++i) {
        pthread_join(pool->threads[i], NULL);
    }

    free(pool->tasks);
    free(pool->threads);
    pthread_mutex_destroy(&pool->queue_mutex);
    pthread_cond_destroy(&pool->queue_not_empty);
    pthread_cond_destroy(&pool->queue_not_full);
    pthread_mutex_destroy(&pool->pool_mutex);
}

int main() {
    thread_pool_t pool;
    thread_pool_init(&pool, INITIAL_THREADS, MAX_THREADS, MAX_TASKS);
    srand(time(NULL));

    printf("Enviando tareas...\n");
    for (int i = 1; i <= 15; ++i) {
        int *arg = malloc(sizeof(int));
        *arg = i;
        thread_pool_submit(&pool, execute_task, arg);
        usleep(200000); // Simular llegadas de tareas con un pequeño retraso
    }

    sleep(10); // Dar tiempo para que las tareas se ejecuten y el pool se redimensione

    thread_pool_destroy(&pool);
    printf("Programa principal terminado.\n");
    return 0;
}

/*
Compila: gcc pthreads6.c -o thread_pool_dynamic -lpthread
Ejecuta: ./thread_pool_dynamic
Explicación:
Este bloque implementa un thread pool que puede redimensionarse dinámicamente.

    -Inicialización: El pool comienza con un número inicial de hilos (INITIAL_THREADS).

    -Envío de Tareas: Cuando se envía una tarea y la cola está llena,
    el pool intenta crear un nuevo hilo trabajador
    (siempre que no se haya alcanzado el número máximo de hilos, MAX_THREADS).

    -Hilos Trabajadores: Los hilos trabajadores toman tareas de la cola y las ejecutan.

    -Redimensionamiento: El redimensionamiento se activa en la función thread_pool_submit
    cuando la cola está llena y todavía hay capacidad para crear más hilos.

Observarás en la salida que el número de hilos trabajadores puede aumentar
a medida que se envían más tareas y la cola se llena,
hasta alcanzar el 1  límite máximo de hilos definido.
Este es un ejemplo básico de cómo un thread pool puede adaptarse a la carga de trabajo.
En una implementación más completa, también se podría incluir la reducción del número de hilos
cuando la carga disminuye.
 */
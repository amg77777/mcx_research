#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_PRIORITY 3
#define MAX_TASKS 30
#define NUM_THREADS 4

typedef struct {
    void (*function)(void *);
    void *argument;
    int priority; // 0: Mayor prioridad, MAX_PRIORITY - 1: Menor prioridad
} task_t;

typedef struct {
    task_t *tasks[MAX_PRIORITY][MAX_TASKS];
    int head[MAX_PRIORITY];
    int tail[MAX_PRIORITY];
    int count[MAX_PRIORITY];
    int capacity;
    pthread_mutex_t queue_mutex;
    pthread_cond_t queue_not_empty[MAX_PRIORITY];
    pthread_cond_t queue_not_full[MAX_PRIORITY];
    pthread_t threads[NUM_THREADS];
    int shutdown;
} thread_pool_t;

void thread_pool_init(thread_pool_t *pool, int num_threads, int max_tasks);
void thread_pool_submit(thread_pool_t *pool, void (*function)(void *), void *argument, int priority);
void thread_pool_destroy(thread_pool_t *pool);
void *worker(void *pool);

void execute_task(void *arg) {
    int task_id = *(int *)arg;
    printf("Hilo %lu ejecutando tarea %d con prioridad %d\n", pthread_self(), task_id, *(int *)(arg + sizeof(int)));
    sleep(rand() % 5);
    free(arg);
}

void thread_pool_init(thread_pool_t *pool, int num_threads, int max_tasks) {
    /*
    Inicializa la estructura del thread pool con soporte para priorización de tareas.

    - Inicializa las cabezas, colas y contadores para cada nivel de prioridad.
    - Inicializa el mutex para proteger el acceso a las colas de tareas.
    - Inicializa las variables de condición 'queue_not_empty' y 'queue_not_full' para cada nivel de prioridad.
    - Establece la capacidad máxima de cada cola de prioridad.
    - Crea y lanza los hilos trabajadores.
    - Inicializa la bandera de 'shutdown' a 0.
    */
}

void thread_pool_submit(thread_pool_t *pool, void (*function)(void *), void *argument, int priority) {
    /*
    Añade una tarea a la cola del thread pool con la prioridad especificada.

    - Bloquea el mutex de la cola.
    - Espera si la cola de la prioridad especificada está llena.
    - Añade la tarea a la cola correspondiente a su prioridad.
    - Incrementa el contador de tareas para esa prioridad.
    - Señala a un hilo trabajador que hay una nueva tarea disponible en esa prioridad.
    - Desbloquea el mutex de la cola.
    */
}

void *worker(void *pool) {
    /*
    Función que ejecuta cada hilo trabajador del pool. Los hilos buscan tareas comenzando por la prioridad más alta.

    - Entra en un bucle infinito para procesar tareas.
    - Bloquea el mutex de la cola.
    - Itera a través de los niveles de prioridad (de mayor a menor).
    - Para cada prioridad, espera si no hay tareas disponibles.
    - Si se encuentra una tarea, la obtiene de la cola correspondiente.
    - Señala que la cola de esa prioridad ya no está llena.
    - Desbloquea el mutex de la cola.
    - Ejecuta la tarea.
    */
}

void thread_pool_destroy(thread_pool_t *pool) {
    /*
    Destruye el thread pool.

    - Bloquea el mutex de la cola.
    - Establece la bandera de 'shutdown' a 1.
    - Envía una señal a todos los hilos trabajadores para que despierten.
    - Desbloquea el mutex de la cola.
    - Espera a que todos los hilos terminen.
    - Libera la memoria asignada.
    - Destruye los mutexes y las condiciones.
    */
}

int main() {
    thread_pool_t pool;
    thread_pool_init(&pool, NUM_THREADS, MAX_TASKS);
    srand(time(NULL));

    printf("Enviando tareas con diferentes prioridades...\n");
    for (int i = 1; i <= 10; ++i) {
        int *arg_low = malloc(sizeof(int) * 2);
        arg_low[0] = i;
        arg_low[1] = 2; // Prioridad baja
        thread_pool_submit(&pool, execute_task, arg_low, 2);

        int *arg_high = malloc(sizeof(int) * 2);
        arg_high[0] = i + 100;
        arg_high[1] = 0; // Prioridad alta
        thread_pool_submit(&pool, execute_task, arg_high, 0);

        int *arg_medium = malloc(sizeof(int) * 2);
        arg_medium[0] = i + 200;
        arg_medium[1] = 1; // Prioridad media
        thread_pool_submit(&pool, execute_task, arg_medium, 1);
    }

    sleep(15);

    thread_pool_destroy(&pool);
    printf("Programa principal terminado.\n");
    return 0;
}

/*
Compila: gcc pthreads9.c -o thread_pool_priority -lpthread
Ejecuta: ./thread_pool_priority
Explicación:
    -Priorización de Tareas:
        Este thread pool avanzado introduce la priorización de tareas.
        Se definen MAX_PRIORITY niveles de prioridad (en este caso, 3: alta, media, baja).

    -Múltiples Colas:
        Internamente, el thread pool gestiona múltiples colas de tareas,
        una para cada nivel de prioridad.

    -thread_pool_submit con Prioridad:
        La función thread_pool_submit ahora toma un argumento adicional de priority
        para indicar la prioridad de la tarea.
        La tarea se encola en la cola correspondiente a su prioridad.

    -worker con Prioridad:
        Los hilos trabajadores, en su función worker,
        buscan tareas comenzando por la prioridad más alta (0)
        y luego pasando a prioridades más bajas.
        Esto asegura que las tareas de mayor prioridad se ejecuten antes que las de menor prioridad.

    -Variables de Condición por Prioridad:
        Se utiliza un array de variables de condición (queue_not_empty y queue_not_full)
        para cada nivel de prioridad, permitiendo que los hilos esperen específicamente por tareas
        en su nivel de prioridad.

Al ejecutar este código, deberías observar que las tareas con prioridad 0 (alta)
tienden a ejecutarse antes que las tareas con prioridad 1 (media) y 2 (baja).
Este es un ejemplo de cómo se puede implementar la priorización en un thread pool
para gestionar diferentes tipos de cargas de trabajo.
 */
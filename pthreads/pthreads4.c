#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    sem_t semaphore;
} rate_limiter_t;

rate_limiter_t* rate_limiter_create(int max_requests);
void rate_limiter_acquire(rate_limiter_t *rl);
void rate_limiter_release(rate_limiter_t *rl);
void rate_limiter_destroy(rate_limiter_t *rl);

rate_limiter_t* rate_limiter_create(int max_requests) {
    /*
    Crea e inicializa un limitador de tasa que permite un máximo de 'max_requests' peticiones simultáneas.

    - Asigna memoria para la estructura del limitador de tasa.
    - Inicializa un semáforo binario (o contador) con un valor inicial de 'max_requests'.
      Este valor representa el número de "permisos" disponibles.
    - Retorna un puntero al limitador de tasa creado.
    */
    rate_limiter_t *rl = malloc(sizeof(rate_limiter_t));
    if (!rl) return NULL;
    if (sem_init(&rl->semaphore, 0, max_requests) != 0) {
        perror("sem_init failed");
        free(rl);
        return NULL;
    }
    return rl;
}

void rate_limiter_acquire(rate_limiter_t *rl) {
    /*
    Intenta adquirir un permiso del limitador de tasa. Si no hay permisos disponibles, el hilo se bloquea hasta que se libere uno.

    - Llama a la función sem_wait(). Esta función decrementa el valor del semáforo.
    - Si el valor del semáforo es mayor que cero, el hilo continúa.
    - Si el valor del semáforo es cero, el hilo se bloquea hasta que otro hilo llame a sem_post().
    */
    sem_wait(&rl->semaphore);
}

void rate_limiter_release(rate_limiter_t *rl) {
    /*
    Libera un permiso al limitador de tasa, incrementando el contador del semáforo.

    - Llama a la función sem_post(). Esta función incrementa el valor del semáforo,
      lo que podría desbloquear a un hilo que estaba esperando en sem_wait().
    */
    sem_post(&rl->semaphore);
}

void rate_limiter_destroy(rate_limiter_t *rl) {
    /*
    Destruye el limitador de tasa, liberando los recursos.

    - Llama a sem_destroy() para liberar los recursos asociados con el semáforo.
    - Libera la memoria asignada para la estructura del limitador de tasa.
    */
    if (rl) {
        sem_destroy(&rl->semaphore);
        free(rl);
    }
}

void *task_function(void *arg) {
    rate_limiter_t *limiter = (rate_limiter_t *)arg;
    printf("Hilo %lu intentando adquirir permiso...\n", pthread_self());
    rate_limiter_acquire(limiter);
    printf("Hilo %lu ha adquirido permiso y está ejecutando la tarea...\n", pthread_self());
    sleep(2); // Simular trabajo
    printf("Hilo %lu ha terminado la tarea y libera el permiso.\n", pthread_self());
    rate_limiter_release(limiter);
    pthread_exit(NULL);
}

int main() {
    int max_requests = 3;
    int num_threads = 5;
    rate_limiter_t *limiter = rate_limiter_create(max_requests);
    if (!limiter) {
        return 1;
    }

    pthread_t threads[num_threads];

    printf("Creando %d hilos...\n", num_threads);
    for (int i = 0; i < num_threads; ++i) {
        if (pthread_create(&threads[i], NULL, task_function, limiter) != 0) {
            perror("Error al crear el hilo");
            rate_limiter_destroy(limiter);
            return 1;
        }
    }

    printf("Esperando que los hilos terminen...\n");
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    rate_limiter_destroy(limiter);
    printf("Programa principal terminado.\n");
    return 0;
}

/*
Compila: gcc pthreads4.c -o thread_pool_basic -lpthread
Ejecuta: ./thread_pool_basic
Explicación:
Este bloque presenta una implementación básica de un thread pool.
Se inicializa un número fijo de hilos trabajadores que esperan tareas en una cola.
La función main envía algunas tareas simples (execute_task) al pool para ser ejecutadas.
Se utilizan mutexes y variables de condición para la sincronización
entre el hilo principal (que encola las tareas)
y los hilos trabajadores (que las desencolan y ejecutan).
Se ha añadido un mecanismo de cierre para que los hilos trabajadores terminen 1
de forma controlada cuando se destruye el pool.

 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define QUEUE_CAPACITY 5

typedef struct {
    int *queue;
    int head;
    int tail;
    int size;
    int capacity;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} blocking_queue_t;

blocking_queue_t* bqueue_create(int capacity);
void bqueue_enqueue(blocking_queue_t *bq, int item);
int bqueue_dequeue(blocking_queue_t *bq);
void bqueue_destroy(blocking_queue_t *bq);

blocking_queue_t* bqueue_create(int capacity) {
    /*
    Crea e inicializa una cola bloqueante con la capacidad especificada.

    - Asigna memoria para la estructura de la cola.
    - Asigna memoria para el array que contendrá los elementos de la cola.
    - Inicializa el mutex para proteger el acceso a la cola.
    - Inicializa las variables de condición 'not_empty' (para señalar cuando hay elementos en la cola)
      y 'not_full' (para señalar cuando hay espacio disponible en la cola).
    - Establece el tamaño, la cabeza y la cola de la cola a sus valores iniciales.
    - Retorna un puntero a la cola bloqueante creada.
    */
}

void bqueue_enqueue(blocking_queue_t *bq, int item) {
    pthread_mutex_lock(&bq->mutex);
    while (bq->size == bq->capacity) {
        pthread_cond_wait(&bq->not_full, &bq->mutex);
    }
    bq->queue[bq->tail] = item;
    bq->tail = (bq->tail + 1) % bq->capacity;
    bq->size++;
    pthread_cond_signal(&bq->not_empty);
    pthread_mutex_unlock(&bq->mutex);
}

int bqueue_dequeue(blocking_queue_t *bq) {
    /*
    Desenfila un elemento de la cola bloqueante. Si la cola está vacía, espera hasta que haya un elemento disponible.

    - Bloquea el mutex para proteger el acceso a la cola.
    - Mientras el tamaño de la cola sea cero:
        - Espera en la variable de condición 'not_empty'. Este hilo se desbloqueará cuando otro hilo
          (el productor) encole un elemento y señale 'not_empty'. El mutex se libera mientras espera.
    - Obtiene el elemento de la cabeza de la cola (en el índice 'head').
    - Decrementa el tamaño de la cola.
    - Actualiza el índice 'head' de forma circular.
    - Señala a un hilo (el productor) que la cola ya no está llena, usando la variable de condición 'not_full'.
    - Desbloquea el mutex.
    - Retorna el elemento desencolado.
    */
}

void bqueue_destroy(blocking_queue_t *bq) {
    /*
    Libera la memoria y destruye el mutex y las variables de condición de la cola bloqueante.
    */
}

void *producer_thread(void *arg) {
    blocking_queue_t *bq = (blocking_queue_t *)arg;
    for (int i = 1; i <= 10; ++i) {
        printf("Productor encolando: %d\n", i);
        bqueue_enqueue(bq, i);
        sleep(rand() % 2);
    }
    pthread_exit(NULL);
}

void *consumer_thread(void *arg) {
    blocking_queue_t *bq = (blocking_queue_t *)arg;
    for (int i = 0; i < 10; ++i) {
        int item = bqueue_dequeue(bq);
        printf("Consumidor desencolando: %d\n", item);
        sleep(rand() % 3);
    }
    pthread_exit(NULL);
}

int main() {
    blocking_queue_t *bq = bqueue_create(QUEUE_CAPACITY);
    if (!bq) {
        perror("Error al crear la cola bloqueante");
        return 1;
    }
    srand(time(NULL));

    pthread_t producer, consumer;

    if (pthread_create(&producer, NULL, producer_thread, bq) != 0) {
        perror("Error al crear el hilo productor");
        bqueue_destroy(bq);
        return 1;
    }

    if (pthread_create(&consumer, NULL, consumer_thread, bq) != 0) {
        perror("Error al crear el hilo consumidor");
        pthread_join(producer, NULL);
        bqueue_destroy(bq);
        return 1;
    }

    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);

    bqueue_destroy(bq);
    printf("Programa principal terminado.\n");
    return 0;
}

/*
Compila: gcc pthreads3.c -o thread_safe_queue -lpthread
Ejecuta: ./thread_safe_queue
Explicación:
Este bloque implementa una cola thread-safe utilizando un mutex
para proteger el acceso a la estructura de la cola
y dos variables de condición (not_empty y not_full)
para coordinar la actividad de los hilos productores y consumidores.
El productor encola números y el consumidor los desencola,
demostrando cómo los hilos pueden comunicarse de forma segura a través de la cola,
incluso cuando la cola está llena o vacía.
 */
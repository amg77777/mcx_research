#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 3
pthread_barrier_t barrier;

void *worker_function(void *arg) {
    /*
    Función que ejecuta cada hilo trabajador.

    - Recibe el ID del hilo como argumento.
    - Imprime un mensaje indicando el inicio de la primera fase del trabajo.
    - Simula la primera fase del trabajo con una pausa aleatoria.
    - Imprime un mensaje indicando que el hilo está esperando en la barrera.
    - Llama a pthread_barrier_wait para esperar a que todos los hilos lleguen a la barrera.
    - Si el valor de retorno de pthread_barrier_wait es PTHREAD_BARRIER_SERIAL_THREAD,
      indica que este hilo es el último en llegar y realiza una acción especial.
    - Imprime un mensaje indicando el inicio de la segunda fase del trabajo.
    - Termina la ejecución del hilo.
    */
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    srand(time(NULL));

    if (pthread_barrier_init(&barrier, NULL, NUM_THREADS) != 0) {
        perror("Error al inicializar la barrera");
        return 1;
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, worker_function, &thread_ids[i]) != 0) {
            perror("Error al crear el hilo");
            pthread_barrier_destroy(&barrier);
            return 1;
        }
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&barrier);
    printf("Programa principal terminado.\n");
    return 0;
}

/*
Compila: gcc pthreads2.c -o thread_barrier -lpthread
Ejecuta: ./thread_barrier
Explicación:
Este bloque muestra cómo utilizar una barrera para sincronizar tres hilos.
Cada hilo realiza una primera fase de trabajo,
luego espera en la barrera hasta que todos los hilos hayan llegado.
Una vez que todos los hilos alcanzan la barrera,
todos se desbloquean y continúan con la segunda fase del trabajo.
El hilo que llega el último puede realizar una acción especial
(indicado por PTHREAD_BARRIER_SERIAL_THREAD).
 */

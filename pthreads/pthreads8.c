#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    int event_occurred;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
} shared_state_t;

void init_shared_state(shared_state_t *state);
void *notifier_thread(void *arg);
void *waiter_thread(void *arg);

void init_shared_state(shared_state_t *state) {
    /*
    Inicializa la estructura del estado compartido, incluyendo el mutex y la variable de condición.

    - Establece la bandera 'event_occurred' a 0 (falso).
    - Inicializa el mutex para proteger el acceso al estado compartido.
    - Inicializa la variable de condición que se utilizará para la señalización.
    */
}

void *notifier_thread(void *arg) {
    /*
    Hilo que realiza alguna tarea y luego señala un evento a través de la variable de condición.

    - Obtiene un puntero a la estructura del estado compartido.
    - Simula alguna cantidad de trabajo.
    - Bloquea el mutex para acceder al estado compartido de forma segura.
    - Establece la bandera 'event_occurred' a 1 (verdadero) para indicar que el evento ha ocurrido.
    - Señala a un único hilo que está esperando en la variable de condición 'condition' usando pthread_cond_signal().
    - Desbloquea el mutex.
    - Termina la ejecución del hilo.
    */
}

void *waiter_thread(void *arg) {
    /*
    Hilo que espera a que ocurra un evento, utilizando una variable de condición.

    - Obtiene un puntero a la estructura del estado compartido.
    - Bloquea el mutex para acceder al estado compartido de forma segura.
    - Mientras la bandera 'event_occurred' sea 0 (el evento aún no ha ocurrido):
        - Espera en la variable de condición 'condition' usando pthread_cond_wait().
          Este hilo se desbloqueará cuando otro hilo (el notificador) señale la condición.
          El mutex se libera automáticamente mientras espera y se vuelve a adquirir al despertar.
    - Una vez que la condición se cumple (la bandera es 1), imprime un mensaje.
    - Desbloquea el mutex.
    - Termina la ejecución del hilo.
    */
}

int main() {
    shared_state_t state;
    init_shared_state(&state);

    pthread_t notifier, waiter;

    printf("Creando hilos...\n");
    if (pthread_create(&notifier, NULL, notifier_thread, &state) != 0) {
        perror("Error al crear el hilo notificador");
        return 1;
    }

    if (pthread_create(&waiter, NULL, waiter_thread, &state) != 0) {
        perror("Error al crear el hilo esperador");
        return 1;
    }

    printf("Esperando que los hilos terminen...\n");
    pthread_join(notifier, NULL);
    pthread_join(waiter, NULL);

    pthread_mutex_destroy(&state.mutex);
    pthread_cond_destroy(&state.condition);

    printf("Programa principal terminado.\n");
    return 0;
}

/*
Compila: gcc pthreads8.c -o condition_signal -lpthread
Ejecuta: ./condition_signal
Explicación:
Este bloque demuestra cómo utilizar variables de condición para la señalización de eventos entre hilos.

    -shared_state_t:
        Esta estructura contiene una bandera (event_occurred)
        para indicar si el evento ha sucedido,
        un mutex para proteger el acceso a esta bandera y una variable de condición (condition)
        para la señalización.

    -notifier_thread:
        Este hilo simula un trabajo y luego,
        una vez que el "evento" ocurre (simulado aquí),
        establece la bandera event_occurred a 1 y señala la variable de condición
        utilizando pthread_cond_signal(). Esto despierta a uno de los hilos que estén esperando
        en esa condición.

    -waiter_thread:
        Este hilo espera a que el evento ocurra.
        Adquiere el mutex y luego entra en un bucle while que verifica la bandera event_occurred.
        Si la bandera es 0, el hilo llama a pthread_cond_wait().
        Esta función libera el mutex y pone al hilo a dormir hasta que se señale la condición.
        Una vez que se señala la condición, el hilo se despierta,
        vuelve a adquirir el mutex y verifica la bandera.
        Cuando la bandera es 1, el hilo sabe que el evento ha ocurrido y puede continuar.

Este patrón es útil para coordinar acciones entre hilos donde un hilo necesita esperar
a que otro hilo complete una tarea o alcance un cierto estado.
 */
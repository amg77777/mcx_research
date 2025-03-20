#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h> // Para open y close

pthread_key_t tls_key;

typedef struct {
    int file_descriptor;
    int *dynamic_memory;
} thread_resources_t;

void cleanup_tls(void *value) {
    /*
    Función de limpieza que se llama automáticamente cuando un hilo termina,
    si se asoció un valor con la clave TLS para ese hilo.

    - Recibe el valor asociado con la clave TLS (que en este caso podría ser
      un puntero a una estructura con recursos del hilo).
    - Libera la memoria asignada dinámicamente por el hilo.
    - Cierra cualquier descriptor de archivo abierto por el hilo.
    */
}

void set_thread_local_data(thread_resources_t *resources) {
    pthread_setspecific(tls_key, resources);
}

thread_resources_t *get_thread_local_data() {
    return (thread_resources_t *)pthread_getspecific(tls_key);
}

void *worker_thread(void *arg) {
    /*
   Función que realiza el trabajo del hilo y comprueba si ha sido cancelado.

   - Imprime un mensaje indicando que el hilo ha comenzado.
   - Establece el estado de cancelación del hilo a habilitado.
   - Establece el tipo de cancelación a asíncrono (la cancelación ocurre inmediatamente).
     (Nota: La cancelación asíncrona puede ser peligrosa; la cancelación diferida es más segura).
   - Registra un handler de limpieza que se ejecutará si el hilo es cancelado.
   - Entra en un bucle que simula trabajo y comprueba si se ha solicitado la cancelación.
   - Al final del bucle, desregistra el handler de limpieza.
   - Termina el hilo.
   */
}

int main() {
    pthread_t worker;
    int thread_id = 1;

    if (pthread_key_create(&tls_key, cleanup_tls) != 0) {
        perror("Error al crear la clave TLS");
        return 1;
    }

    printf("Creando hilo...\n");
    if (pthread_create(&worker, NULL, worker_thread, &thread_id) != 0) {
        perror("Error al crear el hilo trabajador");
        return 1;
    }

    sleep(5);

    printf("Hilo principal: Enviando solicitud de cancelación al hilo trabajador...\n");
    if (pthread_cancel(worker) != 0) {
        perror("Error al cancelar el hilo trabajador");
        return 1;
    }

    printf("Hilo principal: Esperando que el hilo trabajador termine...\n");
    if (pthread_join(worker, NULL) != 0) {
        perror("Error al esperar la terminación del hilo trabajador");
        return 1;
    }

    pthread_key_delete(tls_key);
    printf("Programa principal terminado.\n");
    return 0;
}


/*
Compila: gcc pthreads7.c -o thread_cancellation -lpthread
Ejecuta: ./thread_cancellation
Explicación:
Este bloque muestra cómo cancelar un hilo utilizando pthread_cancel.

    -worker_thread: Esta función realiza un bucle infinito simulando trabajo.

    -pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL):
        Permite que el hilo reciba solicitudes de cancelación.

    -pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL):
        Establece el tipo de cancelación a asíncrono,
        lo que significa que la cancelación puede ocurrir en cualquier momento.
        Es importante tener en cuenta que la cancelación asíncrona puede ser peligrosa
        ya que puede dejar recursos en un estado inconsistente.
        La cancelación diferida (con PTHREAD_CANCEL_DEFERRED
        y puntos de cancelación explícitos como pthread_testcancel())
        es generalmente una forma más segura de manejar la cancelación.

    -pthread_cleanup_push(cleanup_handler, NULL) y pthread_cleanup_pop(1):
        Definen un handler de limpieza. Si el hilo es cancelado,
        la función cleanup_handler se ejecutará antes de que el hilo termine.
        pthread_cleanup_pop(1) desregistra el handler y
        el 1 indica que no se ejecute si el hilo sale normalmente.

    -main:
        El hilo principal crea el hilo trabajador,
        espera unos segundos y luego envía una solicitud de cancelación al hilo trabajador
        usando pthread_cancel().
        Finalmente, espera a que el hilo trabajador termine con pthread_join().

Observarás que después de unos segundos, el hilo principal enviará la solicitud de cancelación
y el hilo trabajador terminará, ejecutando primero la función cleanup_handler.
Recuerda que en aplicaciones reales, la cancelación de hilos debe manejarse con cuidado
para evitar problemas de corrupción de datos o fugas de recursos.
 */
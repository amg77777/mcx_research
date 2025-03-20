#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

pthread_key_t tls_key;

void cleanup_tls(void *value) {
    /*
    Función de limpieza que se llama automáticamente cuando un hilo termina,
    si se asoció un valor con la clave TLS para ese hilo.

    - Recibe el valor asociado con la clave TLS.
    - Imprime un mensaje indicando que se está limpiando la memoria (si es necesario).
    - Libera la memoria asignada para el valor TLS (si fue asignada dinámicamente).
    */
}

void set_thread_local_data(const char *data) {
    /*
    Establece datos específicos del hilo utilizando Thread-Local Storage.

    - Asigna memoria dinámica para almacenar la cadena de datos.
    - Copia la cadena 'data' en la memoria asignada.
    - Asocia la dirección de esta memoria con la clave TLS 'tls_key' para el hilo actual
      utilizando pthread_setspecific().
    */
}

char *get_thread_local_data() {
    /*
    Obtiene los datos específicos del hilo asociados con la clave TLS para el hilo actual.

    - Llama a pthread_getspecific() con la clave TLS 'tls_key'.
    - Retorna el puntero al valor asociado con la clave para este hilo.
      Si no se ha establecido ningún valor, retorna NULL.
    */
}

void *thread_function(void *arg) {
    int thread_id = *(int *)arg;
    char data[50];
    sprintf(data, "Datos del hilo %d", thread_id);

    printf("Hilo %d: Estableciendo datos TLS: %s\n", thread_id, data);
    set_thread_local_data(data);

    printf("Hilo %d: Obteniendo datos TLS: %s\n", thread_id, get_thread_local_data());

    sleep(2);

    printf("Hilo %d: Terminando.\n", thread_id);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[3];
    int thread_ids[3] = {1, 2, 3};

    if (pthread_key_create(&tls_key, cleanup_tls) != 0) {
        perror("Error al crear la clave TLS");
        return 1;
    }

    printf("Creando hilos...\n");
    for (int i = 0; i < 3; ++i) {
        if (pthread_create(&threads[i], NULL, thread_function, &thread_ids[i]) != 0) {
            perror("Error al crear el hilo");
            return 1;
        }
    }

    printf("Esperando que los hilos terminen...\n");
    for (int i = 0; i < 3; ++i) {
        pthread_join(threads[i], NULL);
    }

    pthread_key_delete(tls_key);

    printf("Programa principal terminado.\n");
    return 0;
}


/*
Compila: gcc pthreads5.c -o thread_local_storage -lpthread
Ejecuta: ./thread_local_storage
Explicación:
Este bloque introduce el concepto de Thread-Local Storage (TLS).
Cada hilo en este ejemplo crea y almacena sus propios datos utilizando pthread_setspecific
asociado a una clave (tls_key).
Cuando cada hilo recupera los datos usando pthread_getspecific,
obtiene su propia copia independiente.
La función cleanup_tls se encarga de liberar la memoria que fue asignada para los datos TLS
cuando cada hilo termina.
Esto es útil para evitar el uso de variables globales
y mantener la independencia de los datos por cada hilo.
 */
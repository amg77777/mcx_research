#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_CACHE_SIZE 10

typedef struct {
    char key[50];
    char value[100];
} cache_entry_t;

typedef struct {
    cache_entry_t cache[MAX_CACHE_SIZE]; // array de 10 posiciones, max 10 nodes
    int count;
    pthread_rwlock_t rwlock;
} shared_cache_t;

void cache_init(shared_cache_t *cache);
char *cache_lookup(shared_cache_t *cache, const char *key);
int cache_add(shared_cache_t *cache, const char *key, const char *value);

void cache_init(shared_cache_t *cache) {
    /* Inicializa la estructura de la caché compartida y el read-write lock. */
    cache->count = 0;
    pthread_rwlock_init(&cache->rwlock, NULL);
}

char *cache_lookup(shared_cache_t *cache, const char *key) {
    /* Busca una entrada en la caché de forma segura para múltiples lectores.
    - Adquiere el lock de lectura para la caché.
    - Itera a través de las entradas de la caché.
    - Si encuentra la clave, libera el lock y retorna el valor.
    - Si no se encuentra, libera el lock y retorna NULL.
    */
    int i = -1;
    pthread_rwlock_rdlock(&cache->rwlock);
    while (i++ < cache->count) {
        if (strcmp(cache->cache[i].key, key) == 0) {
            pthread_rwlock_unlock(&cache->rwlock);
            return cache->cache[i].value;
        }
        pthread_rwlock_unlock(&cache->rwlock);
        return(NULL);
    }
}

int cache_add(shared_cache_t *cache, const char *key, const char *value) {
    pthread_rwlock_wrlock(&cache->rwlock);
    if (cache->count < MAX_CACHE_SIZE) {
        strcpy(cache->cache[cache->count].key, key);
        strcpy(cache->cache[cache->count].value, value);
        cache->count++;
        pthread_rwlock_unlock(&cache->rwlock);
        return 0;
    } else {
        pthread_rwlock_unlock(&cache->rwlock);
        return -1;
    }
}

void *reader_thread(void *arg) {
    shared_cache_t *cache = (shared_cache_t *)arg;
    for (int i = 0; i < 5; ++i) {
        char key[50];
        sprintf(key, "key_%d", rand() % 5);
        char *value = cache_lookup(cache, key);
        if (value) {
            printf("Lector %lu: Encontrado key '%s' con valor '%s'\n", pthread_self(), key, value);
        } else {
            printf("Lector %lu: No se encontró key '%s'\n", pthread_self(), key);
        }
        usleep(rand() % 500000);
    }
    pthread_exit(NULL);
}

void *writer_thread(void *arg) {
    shared_cache_t *cache = (shared_cache_t *)arg;
    for (int i = 0; i < 3; ++i) {
        char key[50];
        char value[100];
        sprintf(key, "key_%d", i);
        sprintf(value, "value_%d_%lu", i, pthread_self());
        if (cache_add(cache, key, value) == 0) {
            printf("Escritor %lu: Añadido key '%s' con valor '%s'\n", pthread_self(), key, value);
        } else {
            printf("Escritor %lu: No se pudo añadir key '%s' (caché llena)\n", pthread_self(), key);
        }
        usleep(rand() % 750000); // Espera aleatoria
    }
    pthread_exit(NULL);
}

int main() {
    shared_cache_t cache;
    cache_init(&cache);
    srand(time(NULL));

    pthread_t readers[3], writers[2];

    for (int i = 0; i < 3; ++i) {
        pthread_create(&readers[i], NULL, reader_thread, &cache);
    }
    for (int i = 0; i < 2; ++i) {
        pthread_create(&writers[i], NULL, writer_thread, &cache);
    }

    for (int i = 0; i < 3; ++i) {
        pthread_join(readers[i], NULL);
    }
    for (int i = 0; i < 2; ++i) {
        pthread_join(writers[i], NULL);
    }

    pthread_rwlock_destroy(&cache.rwlock);
    printf("Programa principal terminado.\n");
    return 0;
}

/*
Compila: gcc pthreads1.c -o rwlock_cache -lpthread
Ejecuta: ./rwlock_cache
Explicación:
Este bloque demuestra el uso de read-write locks para proteger una caché compartida.
Múltiples hilos lectores pueden acceder a la caché simultáneamente,
pero cuando un hilo escritor quiere modificar la caché,
necesita obtener un lock exclusivo, bloqueando a todos los demás hilos (lectores y escritores).
 */
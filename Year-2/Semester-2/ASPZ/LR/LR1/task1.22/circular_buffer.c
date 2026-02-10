// Завдання 1.22: Кільцевий буфер (виробник-споживач)
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUF_SIZE 8

typedef struct {
    int data[BUF_SIZE];
    int head;
    int tail;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
} CircularBuffer;

void cb_init(CircularBuffer *cb) {
    cb->head = cb->tail = cb->count = 0;
    pthread_mutex_init(&cb->mutex, NULL);
    pthread_cond_init(&cb->not_full, NULL);
    pthread_cond_init(&cb->not_empty, NULL);
}

void cb_destroy(CircularBuffer *cb) {
    pthread_mutex_destroy(&cb->mutex);
    pthread_cond_destroy(&cb->not_full);
    pthread_cond_destroy(&cb->not_empty);
}

int cb_insert(CircularBuffer *cb, int value) {
    pthread_mutex_lock(&cb->mutex);
    while (cb->count == BUF_SIZE) {
        pthread_cond_wait(&cb->not_full, &cb->mutex);
    }
    cb->data[cb->tail] = value;
    cb->tail = (cb->tail + 1) % BUF_SIZE;
    cb->count++;
    pthread_cond_signal(&cb->not_empty);
    pthread_mutex_unlock(&cb->mutex);
    return 0;
}

int cb_remove(CircularBuffer *cb, int *value) {
    pthread_mutex_lock(&cb->mutex);
    while (cb->count == 0) {
        pthread_cond_wait(&cb->not_empty, &cb->mutex);
    }
    *value = cb->data[cb->head];
    cb->head = (cb->head + 1) % BUF_SIZE;
    cb->count--;
    pthread_cond_signal(&cb->not_full);
    pthread_mutex_unlock(&cb->mutex);
    return 0;
}

void cb_peek(CircularBuffer *cb) {
    pthread_mutex_lock(&cb->mutex);
    printf("[Peek] count=%d: ", cb->count);
    for (int i = 0; i < cb->count; i++) {
        printf("%d ", cb->data[(cb->head + i) % BUF_SIZE]);
    }
    printf("\n");
    pthread_mutex_unlock(&cb->mutex);
}

CircularBuffer shared_buf;
int done = 0;

void *producer(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < 10; i++) {
        int val = id * 100 + i;
        cb_insert(&shared_buf, val);
        printf("[Producer %d] inserted %d\n", id, val);
        usleep(50000);
    }
    return NULL;
}

void *consumer(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < 10; i++) {
        int val;
        cb_remove(&shared_buf, &val);
        printf("[Consumer %d] removed %d\n", id, val);
        usleep(80000);
    }
    return NULL;
}

int main() {
    printf("=== Task 1.22: Circular Buffer ===\n\n");

    printf("--- Single-threaded demo ---\n");
    CircularBuffer buf;
    cb_init(&buf);

    for (int i = 1; i <= 5; i++) cb_insert(&buf, i * 10);
    cb_peek(&buf);

    int val;
    cb_remove(&buf, &val); printf("Removed: %d\n", val);
    cb_remove(&buf, &val); printf("Removed: %d\n", val);
    cb_peek(&buf);

    cb_insert(&buf, 60);
    cb_insert(&buf, 70);
    cb_peek(&buf);
    cb_destroy(&buf);

    printf("\n--- Multi-threaded demo (2 producers, 2 consumers) ---\n");
    cb_init(&shared_buf);

    pthread_t prods[2], cons[2];
    int ids[] = {1, 2};

    for (int i = 0; i < 2; i++) {
        pthread_create(&prods[i], NULL, producer, &ids[i]);
        pthread_create(&cons[i], NULL, consumer, &ids[i]);
    }

    for (int i = 0; i < 2; i++) {
        pthread_join(prods[i], NULL);
        pthread_join(cons[i], NULL);
    }

    cb_destroy(&shared_buf);
    printf("\nDone.\n");
    return 0;
}

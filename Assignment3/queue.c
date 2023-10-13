#include <stdio.h>
#include <stdlib.h>

#define SIZE 10
#define FLAG -1530494976

typedef struct {
    int *array;
    int head;
    int tail;
    int isEmpty;
    int isFull;
} CircularQueue;

void initializeQueue(CircularQueue *cq, int size);
void enqueue(CircularQueue *cq, int ele);
int dequeue(CircularQueue *cq);
int qLength(CircularQueue *cq);
int front(CircularQueue *cq);
void printQueue(CircularQueue *cq);

int main() {
    CircularQueue cq;
    initializeQueue(&cq, SIZE);

    enqueue(&cq, 8);
    enqueue(&cq, 3);
    dequeue(&cq);
    enqueue(&cq, 2);
    enqueue(&cq, 5);
    dequeue(&cq);
    dequeue(&cq);
    enqueue(&cq, 9);
    enqueue(&cq, 1);

    printQueue(&cq);

    free(cq.array);  // Don't forget to free allocated memory
    return 0;
}

void initializeQueue(CircularQueue *cq, int size) {
    cq->array = (int *)malloc(size * sizeof(int));
    cq->head = cq->tail = cq->isEmpty = 0;
    cq->isFull = 0;
}

void enqueue(CircularQueue *cq, int ele) {
    if (cq->head == (cq->tail + 1) % SIZE && cq->isFull) {
        printf("\nerror: overflow\n");
    } else {
        cq->array[cq->tail] = ele;
        cq->isEmpty = 0;
        if (cq->tail == SIZE) {
            cq->tail = 0;
        } else {
            cq->tail += 1;
        }
        if (cq->head == cq->tail) {
            cq->isFull = 1;
        }
    }
}

int dequeue(CircularQueue *cq) {
    if (cq->head == cq->tail && cq->isEmpty == 1) {
        printf("\nerror: underflow\n");
        return FLAG;
    } else {
        int tmp = cq->array[cq->head];
        cq->head = (cq->head + 1) % SIZE;
        if (cq->head == cq->tail) {
            cq->isEmpty = 1;
        }
        return tmp;
    }
}

int qLength(CircularQueue *cq) {
    if (cq->head < cq->tail) {
        return cq->tail - cq->head;
    } else if (cq->head > cq->tail) {
        return SIZE - cq->head + cq->tail;
    } else if (cq->head == cq->tail && cq->isEmpty) {
        return 0;
    } else if (cq->head == cq->tail && cq->isFull) {
        return SIZE;
    } else {
        return FLAG;
    }
}

int front(CircularQueue *cq) {
    if (cq->isEmpty) {
        printf("queue is empty, ");
        return FLAG;
    }
    return cq->array[cq->head];
}

void printQueue(CircularQueue *cq) {
    if (cq->isEmpty) {
        printf("Queue is empty");
    } else if (cq->isEmpty == 0 && cq->head < cq->tail) {
        for (int i = cq->head; i < cq->tail; i++) {
            printf("%d ", cq->array[i]);
        }
    } else if (cq->isEmpty == 0 && cq->head >= cq->tail) {
        for (int i = cq->head; i < SIZE; i++) {
            printf("%d ", cq->array[i]);
        }
        for (int i = 0; i < cq->tail; i++) {
            printf("%d ", cq->array[i]);
        }
    }
}

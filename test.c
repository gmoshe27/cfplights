#include <stdio.h>
#include <pthread.h>

void *test(void *t) {
    int i = 0;
    for (i = 0; i < 10; i++) {
        printf("i = %d\n", i);
    }

    return NULL;
}

int main(void) {
    pthread_t test_thread;

    pthread_create(&test_thread, NULL, test, NULL);
    pthread_join(test_thread, NULL);

    return 0;
}

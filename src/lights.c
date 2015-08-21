#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

#include "state.h"
#include "sixaxis.h"
#include "keyboard.h"

int main(void) {
    State *state;
    pthread_t thread[3];
    int i;

    wiringPiSetup();

    if (initialize_state(&state, INPUT_JOYSTICK) == -1) {
        printf("error: possibly out of memory!\n");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < 3; i++) {
        Sixaxis_Context context;
        context.State = state;
        context.judge = i;

        pthread_create(&thread[i], NULL, sixaxis_thread, (void*)&context);
    }

    /* wait for the input thread and state thread to quit */
    state_loop(state);

    for(i = 0; i < 3; i++) {
        pthread_join(thread[i], NULL);
    }

    release_state(state);
    return 0;
}


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

#include "state.h"
#include "sixaxis.h"
#include "keyboard.h"

int main(void) {
    State *state;
    pthread_t thread;

    wiringPiSetup();

    if (initialize_state(&state, INPUT_JOYSTICK) == -1) {
        printf("error: possibly out of memory!\n");
        exit(EXIT_FAILURE);
    }

    pthread_create(&thread, NULL, sixaxis_thread, (void*)state);

    /* wait for the input thread and state thread to quit */
    state_loop(state);
    pthread_join(thread, NULL);

    release_state(state);
    return 0;
}


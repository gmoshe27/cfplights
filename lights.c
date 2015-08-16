#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

#include "state.h"
#include "sixaxis.h"

void *input_thread(void *state_context) {
    State *state = (State*)state_context;
    char key = 0;

    while(key != 'q') {
        key = getchar();

        switch(key) {
        case 'a':
            set_judgement(state, LEFT, GOOD_LIFT);
            break;
        case 's':
            set_judgement(state, MAIN, GOOD_LIFT);
            break;
        case 'd':
            set_judgement(state, RIGHT, GOOD_LIFT);
            break;
        case 'z':
            set_judgement(state, LEFT, BAD_LIFT);
            break;
        case 'x':
            set_judgement(state, MAIN, BAD_LIFT);
            break;
        case 'c':
            set_judgement(state, RIGHT, BAD_LIFT);
            break;
        case 'm':
            printf("setting state to demo\n");
            set_state(state, STATE_DEMO);
            break;
        case ';':
            printf("setting state to start\n");
            set_state(state, STATE_START);
            break;
        case 'q':
            printf("setting state to exit\n");
            set_state(state, STATE_EXIT);
            break;
        default:
            /* ignore it */
            break;
        }

        usleep(1);
    }

    printf("quitting input thread\n");
    return NULL;
}

int main(void) {
    State *state;
    pthread_t thread;

    wiringPiSetup();

    if (initialize_state(&state) == -1) {
        printf("error: possibly out of memory!\n");
        exit(EXIT_FAILURE);
    }

    pthread_create(&thread, NULL, sixaxis_thread, (void*)state);

    /* wait for the input thread and state thread to quit */
    pthread_join(thread, NULL);
    pthread_join(*(state->thread), NULL);

    release_state(state);

    return 0;
}


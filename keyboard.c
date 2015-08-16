#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "state.h"

void *keyboard_thread(void *state_context) {
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


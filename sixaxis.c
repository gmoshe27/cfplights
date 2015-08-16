#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "sixaxis.h"
#include "state.h"

sixaxis_fd connect_sixaxis(int js_number) {
    int fd = 0;
    char path[50];

    sprintf(path, "/dev/input/js%d", js_number);
    fd = open(path, O_RDONLY | O_NONBLOCK);

    return fd;
}

void disconnect_sixaxis(sixaxis_fd sixaxis) {
    close(sixaxis);
}

int is_connected(sixaxis_fd sixaxis) {
    return sixaxis > 0;
}

int is_button(Sixaxis_Event *event) {
    return (event->type & JS_EVENT_BUTTON) != 0;
}

int is_axis(Sixaxis_Event *event) {
    return (event->type & JS_EVENT_AXIS) != 0;
}

int is_initial_state(Sixaxis_Event *event) {
    return (event->type & JS_EVENT_INIT) != 0;
}

int sample_sixaxis(Sixaxis_Event *event, sixaxis_fd sixaxis) {
    int bytes = read(sixaxis, event, sizeof(*event));

    if (bytes == -1) {
        return 0;
    }

    return bytes == sizeof(*event);
}

void *sixaxis_thread(void *state_context) {
    State *state = (State*)state_context;
    sixaxis_fd jsleft = 0;

    /* wait for a connection from the controller */
    while( (jsleft = connect_sixaxis(0)) == 0) {
        usleep(300);
    }

    if (!is_connected(jsleft)) {
        printf("Could not connect to joystick 0\n");
        exit(EXIT_FAILURE);
    }

    add_connection(state, JUDGE_MAIN);

    Sixaxis_Event event;
    memset(&event, 0, sizeof(Sixaxis_Event));

    int sample = 0;
    while( state->current_state != STATE_EXIT ) {
        usleep(500);

        sample = sample_sixaxis(&event, jsleft);
        if (sample && is_button(&event)) {

            /* we only care when the button is pressed */
            if (!event.value) {
                continue;
            }

            switch(event.number) {
            case BUTTON_SQUARE:
                set_judgement(state, LEFT, GOOD_LIFT);
                break;
            case BUTTON_X:
                set_judgement(state, MAIN, GOOD_LIFT);
                break;
            case BUTTON_CIRCLE:
                set_judgement(state, RIGHT, GOOD_LIFT);
                break;
            case BUTTON_LEFT:
                set_judgement(state, LEFT, BAD_LIFT);
                break;
            case BUTTON_DOWN:
                set_judgement(state, MAIN, BAD_LIFT);
                break;
            case BUTTON_RIGHT:
                set_judgement(state, RIGHT, BAD_LIFT);
                break;
            case BUTTON_PS:
                printf("setting state to demo\n");
                set_state(state, STATE_DEMO);
                break;
            case BUTTON_START:
                printf("setting state to start\n");
                set_state(state, STATE_START);
                break;
            case BUTTON_SELECT:
                printf("setting state to exit\n");
                set_state(state, STATE_EXIT);
                break;
            default:
                /* ignore it */
                break;
            }
        }
    }

    disconnect_sixaxis(jsleft);
    return NULL;
}

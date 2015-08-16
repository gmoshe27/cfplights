#include "state.h"
#include "pins.h"
#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <unistd.h>
#include <string.h>

void reset_judgements();

void *state_thread(void *state_context) {
    State* state = (State*)state_context;

    while(state->current_state != STATE_EXIT) {
 
        if (state->current_state == STATE_INIT) {
            /* do some initialization */
            printf("in state init\n");
            set_lights_off();

            if (state->input_method == INPUT_KEYBOARD) {
                set_state(state, STATE_DEMO);
            }
            else {
                set_state(state, STATE_CONNECTING);
            }
        }
        else if (state->current_state == STATE_CONNECTING) {
            printf("in state connecting\n");
            waiting_for_connections(state);
        }
        else if (state->current_state == STATE_DEMO) {
            printf("in state demo\n");
            demo_mode(state);
        }
        else if (state->current_state == STATE_START) {
            printf("in state start\n");

            set_lights_off();
            delay(500);
            set_lights_on();
            delay(2000);
            set_lights_off();

            reset_judgements(state);
            reset_pins(state->pins);

            set_state(state, STATE_JUDGE);
        }
        else if (state->current_state == STATE_JUDGE) {
            /* wait for all entries to be submitted */
            printf("in state judge\n");
            wait_for_judgements(state);
        }
        else if (state->current_state == STATE_RESULTS) {
            printf("in state results\n");
            display_results(state);
        }

        usleep(1);
    }

    set_lights_off();

    printf("quitting state thread\n");
    return NULL;
}

int initialize_state(State **state, int input_method) {
    *state = (State*)malloc(sizeof(State));
    pthread_t thread;

    if (state == NULL && *state == NULL) {
        return -1;
    }

    memset(*state, 0, sizeof(State));

    (*state)->current_state = STATE_INIT;
    (*state)->input_method = input_method;

    int create_status = pthread_create(&thread, NULL, state_thread, (void*)(*state));
    (*state)->thread = &thread;
    if (create_status) {
        return create_status;
    }

    if (pthread_mutex_init(&(*state)->lock, NULL)) {
        return -1;
    }

    if (initialize_pins(&(*state)->pins)) {
        return -1;
    }

    reset_judgements(*state);
    
    return 0;
}

void release_state(State *state) {
    pthread_mutex_destroy(&state->lock);

    if (state->thread != NULL) {
        pthread_join(*(state->thread), NULL);
    }

    if (state != NULL) {
        free(state);
    }
}

void set_state(State *state, int new_state) {
    pthread_mutex_lock(&state->lock);
    state->current_state = new_state;
    pthread_mutex_unlock(&state->lock); 
}

void demo_mode(State *state) {
    set_lights_off();

    /* do cool stuff */
    while (state->current_state == STATE_DEMO) {
        for (int i = 0; i < 6; i++) {
            digitalWrite(i, HIGH);
            delay(200);
            digitalWrite(i, LOW);
            delay(200);
        }
    }
}

void waiting_for_connections(State *state) {
    while(state->current_state == STATE_CONNECTING && state->connection_count != 1) {
        printf("start wiating for connections 1 2 3...\n");
        digitalWrite(0, HIGH);
        delay(200);
        digitalWrite(0, LOW);
        delay(200);
    }

    printf("all judges connected\n");
    set_state(state, STATE_DEMO);
}

void start_lift(State *state) {
    /* set all pins high for 1.5 seconds */
    /* reset the pins */
    /* clear out all of the judge values */
    /* send vibration to all judges */
}

void wait_for_judgements(State *state) {
    /* wait for judge input */
    while(state->current_state == STATE_JUDGE) {
        printf("waiting for judgements\n");
        if (state->judgements[0] && state->judgements[1] && state->judgements[2]) {
            set_state(state, STATE_RESULTS);
            break;
        }
        sleep(1);
    }
}

void display_results(State *state) {
    /* output the results for 1.5 minutes */
    int i = 0;
    for (i = 0; i < 6; i++) {
       digitalWrite(i, state->pins[i]);
    }

    /* hold indefinitely */
    while(state->current_state == STATE_RESULTS) {
        sleep(1);
    }
}

void set_judgement(State *state, int judge, int judgement) {
    pthread_mutex_lock(&state->lock);
    
    /* if a judge entered their judgement do not allow any chagnes to that judgement */
    if (state->judgements[judge] == 0) {
        state->judgements[judge] = 1;
        set_pin(state->pins, judge, judgement);
    }

    pthread_mutex_unlock(&state->lock);
}

void reset_judgements(State *state) {
    pthread_mutex_lock(&state->lock);

    state->judgements[0] = 0;
    state->judgements[1] = 0;
    state->judgements[2] = 0;

    pthread_mutex_unlock(&state->lock);
}

void add_connection(State *state, int judge) {
    pthread_mutex_lock(&state->lock);
    state->connection_count += 1;
    state->connected_joystick[judge] = 1;
    pthread_mutex_unlock(&state->lock);
}

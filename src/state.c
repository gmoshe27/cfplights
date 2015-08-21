#include "state.h"
#include "pins.h"
#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <unistd.h>
#include <string.h>

void reset_judgements();

void state_loop(State *state) {
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
    printf("quitting state loop\n");
}

int initialize_state(State **state, int input_method) {
    *state = (State*)malloc(sizeof(State));

    if (state == NULL && *state == NULL) {
        return -1;
    }

    memset(*state, 0, sizeof(State));

    (*state)->current_state = STATE_INIT;
    (*state)->input_method = input_method;

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

void prompt_judge_to_connect(int judge_pin) {
    /* blink the lights to prompt the judge to connect */
    digitalWrite(judge_pin, HIGH);
    digitalWrite(judge_pin + 3, HIGH);
    delay(200);
    digitalWrite(judge_pin, LOW);
    digitalWrite(judge_pin + 3, LOW);
    delay(200);    
}

void waiting_for_connections(State *state) {
    printf("start waiting for connections 1 2 3...\n");
    while(state->current_state == STATE_CONNECTING && state->connection_count != 3) {
        if (state->connected_joystick[JUDGE_LEFT] == 0) {
            prompt_judge_to_connect(LEFT);
        } else if (state->connected_joystick[JUDGE_MAIN] == 0) {
            digitalWrite(LEFT, HIGH);
            digitalWrite(LEFT + 3, HIGH);
            prompt_judge_to_connect(MAIN);
        } else {
            digitalWrite(MAIN, HIGH);
            digitalWrite(MAIN + 3, HIGH);
            prompt_judge_to_connect(RIGHT);
        }
    }

    printf("all judges connected\n");
    set_state(state, STATE_DEMO);
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

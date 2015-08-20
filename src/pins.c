#include "pins.h"
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>

int initialize_pins(int **pins) {
    int i =0;

    *pins = (int*)malloc(6 * sizeof(int));
    if (pins == NULL) {
        return -1;
    }

    for (i = 0; i < 6; i++) {
        pinMode(i, OUTPUT);
        (*pins)[i] = LOW;
    }

    return 0;
}

void set_lights_off(void) {
    int i = 0;
    for (i = 0; i < 6; i++) {
        digitalWrite(i, LOW);
    }
}

void set_lights_on(void) {
    int i = 0;
    for (i = 0; i < 6; i++) {
        digitalWrite(i, HIGH);
    }
}

void toggle_pin_state(int *pins, int pin) {
    pins[pin] = pins[pin] ^ HIGH;
}

void set_pin(int *pins, int judge, int judgement) {
    int offset = 3 * judgement;
    pins[judge + offset] = HIGH;
}

void reset_pins(int *pins) {
    int i = 0;
    for (i = 0; i < 6; i++) {
        pins[i] = LOW;
    }
}

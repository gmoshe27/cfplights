#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "sixaxis.h"

/* attributions go to: https://github.com/drewnoakes/joystick */

typedef int sixaxis_fd;

#define JS_EVENT_BUTTON 0x01 // button pressed/released
#define JS_EVENT_AXIS   0x02 // joystick moved
#define JS_EVENT_INIT   0x80 // initial state of device

typedef struct {
    unsigned int time;      /* ts of the event in ms */
    short value;            /* value of the js event */
    unsigned char type;     /* event type */
    unsigned char number;   /* axis/button number */
} Sixaxis_Event;

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

int main(void) {
    sixaxis_fd jsleft = connect_sixaxis(0);

    if (!is_connected(jsleft)) {
        printf("Could not connect to joystick 0\n");
        exit(EXIT_FAILURE);
    }

    Sixaxis_Event event;
    memset(&event, 0, sizeof(Sixaxis_Event));

    while( !(event.number == 16 && event.value == 1) ) {
        usleep(500);
        
        if (sample_sixaxis(&event, jsleft)) {
            if (is_button(&event)) {
                printf("Button %u is %s\n", event.number, event.value == 0 ? "up" : "down");
            }
            else if (is_axis(&event)) {
                printf("Axis %u is at position %d\n", event.number, event.value);
            }
        }
    }
    
    disconnect_sixaxis(jsleft);
    return 0;
}

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

sixaxis_fd connect_sixaxis(int js_number);

void disconnect_sixaxis(sixaxis_fd sixaxis);

int is_connected(sixaxis_fd sixaxis);

int is_button(Sixaxis_Event *event);

int is_axis(Sixaxis_Event *event);

int is_initial_state(Sixaxis_Event *event);

int sample_sixaxis(Sixaxis_Event *event, sixaxis_fd sixaxis);


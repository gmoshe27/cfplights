/* attributions go to: https://github.com/drewnoakes/joystick */

typedef int sixaxis_fd;

#define JS_EVENT_BUTTON 0x01 // button pressed/released
#define JS_EVENT_AXIS   0x02 // joystick moved
#define JS_EVENT_INIT   0x80 // initial state of device

#define BUTTON_SELECT   0
#define BUTTON_START    3
#define BUTTON_UP       4
#define BUTTON_RIGHT    5
#define BUTTON_DOWN     6
#define BUTTON_LEFT     7
#define BUTTON_L2       8
#define BUTTON_R2       9
#define BUTTON_L1       10
#define BUTTON_R1       11
#define BUTTON_TRIANGLE 12
#define BUTTON_CIRCLE   13
#define BUTTON_X        14
#define BUTTON_SQUARE   15
#define BUTTON_PS       16

typedef struct {
    unsigned int time;      /* ts of the event in ms */
    short value;            /* value of the js event */
    unsigned char type;     /* event type */
    unsigned char number;   /* axis/button number */
} Sixaxis_Event;

typedef struct {
    void 	*state;
    int 	judge; /* judge 0 is left, judge 1 is main, judge 2 is right */
} Sixaxis_Context;

sixaxis_fd connect_sixaxis(int js_number);

void disconnect_sixaxis(sixaxis_fd sixaxis);

int is_connected(sixaxis_fd sixaxis);

int is_button(Sixaxis_Event *event);

int is_axis(Sixaxis_Event *event);

int is_initial_state(Sixaxis_Event *event);

int sample_sixaxis(Sixaxis_Event *event, sixaxis_fd sixaxis);

void *sixaxis_thread(void *sixaxis_context);


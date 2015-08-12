#define WHITE_LEFT    0
#define WHITE_MAIN    1
#define WHITE_RIGHT   2
#define RED_LEFT      3
#define RED_MAIN      4
#define RED_RIGHT     5

#define LEFT    0
#define MAIN    1
#define RIGHT   2

#define GOOD_LIFT 0
#define BAD_LIFT  1

int initialize_pins(int **pins);

void set_lights_on(void);

void set_lights_off(void);

void toggle_pin_state(int *pins, int pin);

void read_input(int *pins);

void set_pin(int *pins, int judge, int judgement);

void reset_pins(int *pins);

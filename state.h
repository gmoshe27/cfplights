#include "pins.h"
#include <pthread.h>

#define STATE_INIT 0
#define STATE_DEMO 1
#define STATE_START 2
#define STATE_JUDGE 3
#define STATE_RESULTS 4
#define STATE_EXIT 5

typedef struct {
    int                 current_state;
    pthread_mutex_t     lock;
    pthread_t           *thread;
    int                 *pins;
    int                 judgements[3];
} State;

int initialize_state(State **state);

void release_state(State *state);

void set_state(State *state, int new_state);

void demo_mode(State *state);

void start_lift(State *state);

void wait_for_judgements(State* state);

void display_results(State *state); 

void set_judgement(State *state, int judge, int judgement);

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>

#include <wiringPi.h>
#include "state.h"
#include "sixaxis.h"
#include "keyboard.h"

//using namespace std;

#define DAEMON_NAME "cfplightsd"

void process(){
    State *state;
    pthread_t thread;

    syslog (LOG_NOTICE, "starting cfplightsd");
    wiringPiSetup();

    if (initialize_state(&state, INPUT_JOYSTICK) == -1) {
        syslog (LOG_PERROR, "possibly out of memory");
        exit(EXIT_FAILURE);
    }

    pthread_create(&thread, NULL, sixaxis_thread, (void*)state);

    /* run the main loop */
    state_loop(state);

    /* wait for the input thread and state thread to quit */
    pthread_join(thread, NULL);
    release_state(state);
}

int main(int argc, char *argv[]) {

    //Set our Logging Mask and open the Log
    setlogmask(LOG_UPTO(LOG_NOTICE));
    openlog(DAEMON_NAME, LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID, LOG_USER);

    syslog(LOG_INFO, "Entering Daemon");

    pid_t pid, sid;

    //Fork the Parent Process
    pid = fork();

    if (pid < 0) { exit(EXIT_FAILURE); }

    //We got a good pid, Close the Parent Process
    if (pid > 0) { exit(EXIT_SUCCESS); }

    //Change File Mask
    umask(0);

    //Create a new Signature Id for our child
    sid = setsid();
    if (sid < 0) { exit(EXIT_FAILURE); }

    //Change Directory
    //If we cant find the directory we exit with failure.
    if ((chdir("/")) < 0) { exit(EXIT_FAILURE); }

    //Close Standard File Descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    //----------------
    //Main Process
    //----------------
    process();

    syslog (LOG_NOTICE, "Leaving Daemon");

    //Close the log
    closelog ();
}

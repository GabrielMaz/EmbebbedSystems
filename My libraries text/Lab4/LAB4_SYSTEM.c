/*** BeginHeader */

#define CLEAR_SCREEN() (printf(" \x1Bt"));
#define WAIT() (semaphore -= 1);
#define SIGNAL() (semaphore += 1);
#define CONSOLE 1
#define ETHERNET 0
#define MAX_INTERFACES 2

#use LAB3_IO.LIB
#use LAB3_EVENT.LIB

enum STATE {
    MENU = 0,
    DISPLAY_HOUR = 1,
    INPUT_HOUR = 2,
    LIST_EVENTS = 3,
    ADD_EVENT = 4,
    DELETE_EVENT = 5,
    ANALOG_INPUT_0 = 6,
    ANALOG_INPUT_1 = 7,
    INITIAL = 8
};

int current_state;
int semaphore;

/*** EndHeader */

/*** BeginHeader initSystem */
void initSystem();
/*** EndHeader */

void initSystem() {
    configurePorts();
    initEvents();
    setState(INITIAL);
    events_actived = 0;
    semaphore = 1;
}

/*** BeginHeader setState */
void setState(enum STATE newState);
/*** EndHeader */

void setState(enum STATE newState) {
    current_state = newState;
}

/*** BeginHeader getState */
enum STATE getState();
/*** EndHeader */

enum STATE getState() {
    return current_state;
}

/*** BeginHeader delayMS */
void delayMS(int ms_delay);
/*** EndHeader */

void delayMS(int ms_delay) {
    auto unsigned long t0;

    // ms timer is updated by rabbit
    t0 = MS_TIMER;

    while (MS_TIMER < (t0 + ms_delay));
}
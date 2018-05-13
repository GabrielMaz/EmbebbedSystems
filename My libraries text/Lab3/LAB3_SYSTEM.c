/*** BeginHeader */

#define CLEAR_SCREEN() (printf(" \x1Bt"));

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
    ANALOG_INPUT_1 = 7
};

int current_state;

/*** EndHeader */

/*** BeginHeader init */
void init();
/*** EndHeader */

void init() {
    setState(MENU);
    events_actived = 0;
    configurePorts();
    initEvents();
    setState(MENU);
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
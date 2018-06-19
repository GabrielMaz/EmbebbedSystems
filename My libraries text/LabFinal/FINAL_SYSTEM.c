/*** BeginHeader */

#define CLEAR_SCREEN() (printf(" \x1Bt"));
#define MAX_INTERFACES 2

#use FINAL_IO.LIB
#use FINAL_EVENT.LIB

enum STATE {
    INITIAL = 0,
    MENU = 1,
    LIST_CONTACTS = 2,
    ADD_CONTACT = 3,
    DELETE_CONTACT = 4,
    LOCATION = 5,
};

int current_state;

/*** EndHeader */

/*** BeginHeader initSystem */
void initSystem();
/*** EndHeader */

void initSystem() {
    configurePorts();
    printf("Iniciando Socket\n");
	sock_init();
	printf("Iniciado\n");
    initAgenda();
    setState(INITIAL);
    events_actived = 0;
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
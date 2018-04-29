/*** BeginHeader */

#define CLEAR_SCREEN() (printf(" \x1Bt"));
#define MAX_NUMBER_EVENTS 7

typedef struct Event {
    int array_postion;
    char name[11];
    char leds[9];
    unsigned long time;
};

struct Event events[MAX_NUMBER_EVENTS];
int events_actived;
/*** EndHeader */

/*** BeginHeader initEvents */
void initEvents();
/*** EndHeader */

void initEvents() {
    int i;
    struct Event event;

    for (i = 0; i < MAX_NUMBER_EVENTS; i++){
        event.array_postion = -1;
        events[i] = event;
    }
}

/*** BeginHeader printEvents */
cofunc void printEvents();
/*** EndHeader */

cofunc void printEvents() {
    struct Event event;
    int i;

    CLEAR_SCREEN();

    printf("Listado de eventos:\n\n");

    for (i = 0; i < MAX_NUMBER_EVENTS; i++){
        event = events[i];

        if (event.array_postion != -1) {
            printf("-------- %d --------\n\n", i+1);
            printf("Nombre: ");
            printEventData(event.name);
            printf("\n\n");
            printf("Leds: ");
            printEventData(event.leds);
            printf("\n\n");
            displayHourUI(event.time);
            printf("\n");
        }
    }

    printf("\n\n");
}

/*** BeginHeader printEventData */
void printEventData(char data[]);
/*** EndHeader */

void printEventData(char data[]) {
    int i;

    i = 0;

    while((data[i]) != '\0'){
        printf("%c", data[i]);
        i++;
    }    
}

/*** BeginHeader createEventUi */
cofunc void createEventUi();
/*** EndHeader */

cofunc void createEventUi() {
    struct Event event, *event_pointer;
    struct tm time, *time_pointer;
    int leds_validate;

    time_pointer = &time;
    event_pointer = &event;

    CLEAR_SCREEN();

    // Ask for a event name
    wfd getEventName(&event.name);

    printf("\n\n");

    // Ask for leds value
    wfd getEventLeds(&event.leds);

    // check if entered value belongs to the possible values
    if (!validateEventLeds(&event.leds)) {
        CLEAR_SCREEN();
        printf("Por favor ingrese un dato valido\n\n");
        abort;
    }

    // Ask for time and date
    wfd askTimeHourData(&event.time, time_pointer, 1, 1);

    insertEvent(event_pointer);

    CLEAR_SCREEN();
    printf("Su evento fue agregado a la lista de eventos programados\n\n");
    setState(MENU);
}

/*** BeginHeader insertEvent */
void insertEvent(struct Event *event);
/*** EndHeader */

void insertEvent(struct Event *event) {
    int i;

    for (i = 0; i < MAX_NUMBER_EVENTS; i++) {
        if (events[i].array_postion == -1) {
            (*event).array_postion = i;
            events[i] = (*event);
            events_actived += 1;
            break;
        }
    }
}

/*** BeginHeader getEventName */
cofunc void getEventName(char name[]);
/*** EndHeader */

cofunc void getEventName(char name[]) {

    printf("Por favor ingrese un nombre para el evento (maximo 10 caracteres) : ");

    // get name
    waitfor(getswf(name));
}

/*** BeginHeader getStringName */
char getStringName(char *name);
/*** EndHeader */

char getStringName(char *name) {
    char result[11];
    int i;

    i = 0;

    while (*(name+i) != '\0') {
        result[i] = *(name+i);
        i++;
    }
    return result;
}

/*** BeginHeader getEventLeds */
cofunc void getEventLeds(char* leds);
/*** EndHeader */

cofunc void getEventLeds(char* leds) {

    printf("Por favor ingrese una cadena de largo 8 de 0 o 1 para la salida de los leds: \n\n");

    waitfor(getswf(leds));
}

/*** BeginHeader */
void printCharInbin(char data);
/*** EndHeader */

void printCharInbin(char data) {
    char a, i;

    a = data;
    for (i = 0; i < 8; i++) {
        printf("%d", !!((a << i) & 0x80));
    }
    printf("\n\n");
}

/*** BeginHeader validateEventLeds */
int validateEventLeds(char *leds);
/*** EndHeader */

int validateEventLeds(char *leds) {
    char aux;
    int i, result;

    i = 0;
    result = 1;

    aux = *leds;
    for (i = 0; i < 8; i++) {

        if (!!((aux << i) & 0x80) != 1 & !!((aux << i) & 0x80) != 0) {
            return 0;
        }
    }

    return result;
}

/*** BeginHeader deleteEventUI */
cofunc void deleteEventUI();
/*** EndHeader */

cofunc void deleteEventUI() {
    char data[4];
    int option, i;

    printf("Ingrese numero de evento a eliminar: ");
    waitfor(getswf(data));

    option = converter(data) - 1;

    if (option < 0 | option > 5) {
        printf("Por favor ingrese un dato valido\n\n");
        abort;
    }

    deleteEvent(option);    

    CLEAR_SCREEN();
    printf("Se elimino correctamente el evento\n\n");
}

/*** BeginHeader deleteEvent */
void deleteEvent(int option);
/*** EndHeader */

void deleteEvent(int option) {
    int i;
    for (i = 0; i < MAX_NUMBER_EVENTS; i++) {
        if (i == option) {
            events[i].array_postion = -1;
            events_actived -= 1;
        }
    }
}

/*** BeginHeader checkEvents */
void checkEvents();
/*** EndHeader */

void checkEvents() {
    int i;
    struct tm time, *time_pointer, event_time, *event_time_pointer;
    unsigned long time_in_sec;
    int show;

    time_in_sec = getRtcTime();

    time_pointer = &time;
    event_time_pointer = &event_time;

    // convert to time structure
    mktm(time_pointer, time_in_sec);

    if (events_actived > 0) {
        for (i = 0; i < MAX_NUMBER_EVENTS; i++) {
            if (events[i].array_postion != -1){

                mktm(event_time_pointer, events[i].time);

                if (event_time.tm_year == time.tm_year & event_time.tm_mon == time.tm_mon & event_time.tm_mday == time.tm_mday 
                    & event_time.tm_hour == time.tm_hour & event_time.tm_min == time.tm_min & event_time.tm_sec == time.tm_sec) {
                    
                    WrPortI(PORT_A, getPortShadow(PORT_A), turnOnLeds(events[i].leds));

                    deleteEvent(events[i].array_postion);                    
                }
                break;
            }
        }
    }
}

/*** BeginHeader turnOnLeds */
int turnOnLeds(char *leds);
/*** EndHeader */

int turnOnLeds(char *leds) {
    int i;
    char mask, result;

    mask = 0x80;
    result = 0x00;

    for (i=0; i<8; i++) {
        if ((*(leds+i) - 0x30) == 1) {
            result = result | (mask >> i);
        }
    }
    return result;
}
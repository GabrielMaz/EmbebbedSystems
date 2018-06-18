/*** BeginHeader */

#use FINAL_SYSTEM.LIB
#use FINAL_MENU.LIB
#use FINAL_ETHERNET.LIB
#use FINAL_UCOS.LIB

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
void printEvents();
/*** EndHeader */

void printEvents() {
    struct Event event;
    int i;

    CLEAR_SCREEN();

    if (events_actived == 0) {
        printf("No hay eventos para mostrar\n\n");

    } else {
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

/*** BeginHeader printEventsEthernet */
void printEventsEthernet();
/*** EndHeader */

void printEventsEthernet() {
    struct Event event;
    int i;
    char result[80];

    if (events_actived == 0) {
        printEthernet("No hay eventos programados\n\n");

    } else {
        printEthernet("Listado de eventos:\n\n");

        for (i = 0; i < MAX_NUMBER_EVENTS; i++){
            event = events[i];

            if (event.array_postion != -1) {
                sprintf(result, "-------- %d --------\n\nNombre: ", i+1);
                printEventDataEthernet(event.name, result);
                strcat(result, "\n\nLeds:");
                printEventLedsEthernet(event.leds, result);
                strcat(result, "\n");
                printEthernet(result);
                displayHourUI(event.time);
            }
        }        
    }
}

/*** BeginHeader printEventDataEthernet */
void printEventDataEthernet(char data[], char resultEthernet[]);
/*** EndHeader */

void printEventDataEthernet(char data[], char resultEthernet[]) {
    int i;
    char result[11];

    i = 0;

    while((data[i]) != '\0'){ 
        result[i] = data[i];
        i++;
        if (i == 10) {
            break;
        }
    }
    strcat(resultEthernet, result);
}

/*** BeginHeader printEventLedsEthernet */
void printEventLedsEthernet(char data[], char resultEthernet[]);
/*** EndHeader */

void printEventLedsEthernet(char data[], char resultEthernet[]) {
    int i;
    char result[9];

    for (i = 0; i < 8; i++) {
        result[i] = data[i];
    }

    strcat(resultEthernet, result);
}

/*** BeginHeader createEventUi */
void createEventUi();
/*** EndHeader */

void createEventUi() {
    struct Event event, *event_pointer;
    struct tm time, *time_pointer;
    int time_validated, date_validate;
    char *name_pointer, *leds_pointer;

    time_pointer = &time;
    event_pointer = &event;
    name_pointer = (*event_pointer).name;
    leds_pointer = (*event_pointer).leds;

    time_validated = 1;
    date_validate = 1;

    if (events_actived == MAX_NUMBER_EVENTS) {
        clearScreenEthernet();
        printEthernet("No hay espacio para mas eventos\n\n");

    } else {
        // Ask for a event name
        getEventName(name_pointer);

        // Ask for leds values
        getEventLeds(leds_pointer);

        // check if entered value belongs to the possible values
        if (!validateEventLeds(leds_pointer)) {
            clearScreenEthernet();
            printEthernet("Por favor ingrese un dato valido");
            
            return;
        }
        
        // Ask for time and date
        askTimeHourDataEthernet(&event.time, time_pointer, 1, 1, &time_validated, &date_validate);

        if (!(time_validated & date_validate)) {
            return;
        }

        insertEvent(event_pointer);

        clearScreenEthernet();
        printEthernet("Su evento fue agregado a la lista de eventos programados");
        clearScreenEthernet();
    }
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
void getEventName(char *name);
/*** EndHeader */

void getEventName(char *name) {
    printEthernet("\nPor favor ingrese un nombre para el evento (maximo 10 caracteres): ");
    CLEAR_BUFFER();
    while(!sock_gets(&socket, name, 11)) { DELAY100MS(); }
    
    CLEAR_SOCKET();
    
    sock_err:
    switch(status) {
        case 1: /* foreign host closed */
            printf("User closed session\n");
            break;
        case -1: /* time-out */
            printf("Connection timed out\n");
            break;
    }
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
void getEventLeds(char* leds);
/*** EndHeader */

void getEventLeds(char* leds) {

    printEthernet("\nPor favor ingrese una cadena de largo 8 de 0 o 1 para la salida de los leds: ");
    CLEAR_BUFFER();

    while(!(sock_gets(&socket, leds, 9))) { DELAY100MS(); }

    CLEAR_SOCKET();

    sock_err:
    switch(status) {
        case 1: /* foreign host closed */
            printf("User closed session\n");
            break;
        case -1: /* time-out */
            printf("Connection timed out\n");
            break;
    }
}

/*** BeginHeader printCharInbin */
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

/*** BeginHeader deleteEventEthernetUI */
void deleteEventEthernetUI();
/*** EndHeader */

void deleteEventEthernetUI() {
    char data[4];
    int option, i;

    printEthernet("\nIngrese numero de evento a eliminar: ");
    CLEAR_BUFFER();

    while(!sock_gets(&socket, data, 4)) { DELAY100MS(); }
    option = converter(data) - 1;

    CLEAR_SOCKET();

    /*option = converter(buffer_msg) - 1;
    CLEAR_BUFFER(); */
    
    sock_err:
    switch(status) {
        case 1: /* foreign host closed */
            printf("User closed session\n");
            break;
        case -1: /* time-out */
            printf("Connection timed out\n");
            break;
    }

    if (option < 0 | option > 5) {
        clearScreenEthernet();
        printEthernet("Por favor ingrese un dato valido ");
        return;
    }

    if (deleteEvent(option)) {
        clearScreenEthernet();
        printEthernet("Se elimino correctamente el evento");

    } else {
        clearScreenEthernet();
        printEthernet("No existe un evento con ese indice");
    }    
}

/*** BeginHeader deleteEvent */
int deleteEvent(int option);
/*** EndHeader */

int deleteEvent(int option) {
    int i, deleted;

    deleted = 0;

    for (i = 0; i < MAX_NUMBER_EVENTS; i++) {
        if (i == option & events[i].array_postion != -1) {
            events[i].array_postion = -1;
            events_actived -= 1;
            deleted = 1;
        }
    }

    return deleted;
}

/*** BeginHeader checkEventsActivated */
void checkEventsActivated();
/*** EndHeader */

void checkEventsActivated() {
    int i;
    struct tm time, *time_pointer, event_time, *event_time_pointer;
    unsigned long time_in_sec;
    char *shadow;

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

                    shadow = getPortShadow(PORT_A);
                    
                    WrPortI(PORT_A, shadow, turnOnLeds(events[i].leds));

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
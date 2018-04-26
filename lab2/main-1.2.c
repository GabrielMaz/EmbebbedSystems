#define CLEAR_SCREEN() (printf(" \x1Bt"));
#define MAX_NUMBER_EVENTS 7

enum ACTIONS {
    SET_LED_STATUS
};

enum STATE {
    MENU = 0,
    DISPLAY_HOUR = 1,
    INPUT_HOUR = 2,
    LIST_EVENTS = 3,
    ADD_EVENT = 4,
    DELETE_EVENT = 5
};

typedef struct Event {
    int array_postion;
    char name[11];
    char leds[9];
    unsigned long time;
};

int current_state;
struct Event events[MAX_NUMBER_EVENTS];
int events_actived;

// ---------------------------------- SYSTEM ----------------------------------
void init();
enum STATE getState();
void setState(enum STATE newState);
void initEvents();

// ---------------------------------- MENU ----------------------------------
void menuUI();
cofunc void optionSelected();
int converter(char *data);
int converterBin(char *data);
int multipleOfTen(int multiple);
int multipleOfTwo(int multiple);

// ---------------------------------- DATE AND TIME ----------------------------------
unsigned long getRtcTime();
void displayHourUI(unsigned long time_in_sec);
void inputHourUI();
cofunc void setDate();
cofunc void askTimeHourData(unsigned long *time_in_sec, struct tm *time_pointer, int ask_time, int ask_date);
cofunc void askForDateTime(unsigned long* time_in_sec);
cofunc void getTime(struct tm *time_pointer);
int validateTime(int hour_int, int min_int, int sec_int);
cofunc void getDate(struct tm *time_pointer);
int validateDate(int day_int, int month_int, int year_int);
void setClock(unsigned long *time_in_sec, struct tm *time_pointer);

// ---------------------------------- EVENT ----------------------------------
cofunc void printEvents();
void printEventData(char name[]);
cofunc void createEventUi();
void insertEvent(struct Event *event);
cofunc void getEventName(char name[]);
char getStringName(char *name);
cofunc void getEventLeds(char *leds);
void printCharInbin(char data);
int validateEventLeds(char *leds);
cofunc void deleteEventUI();

// Main loop function
void main () {
    int input, data_valid;
    unsigned long time;

    init();
    current_state = getState();

    while (1) {
        costate {
            switch(current_state){
                case (MENU):
                    menuUI();
                    wfd optionSelected();
                    break;

                case (DISPLAY_HOUR):
                    displayHourUI(getRtcTime());
                    setState(MENU);
                    break;

                case (INPUT_HOUR):
                    inputHourUI();
                    wfd setDate();
                    break;

                case (LIST_EVENTS):
                    wfd printEvents();
                    setState(MENU);
                    break;

                case (ADD_EVENT):
                    if (events_actived < MAX_NUMBER_EVENTS) {
                        wfd createEventUi();
                    } else {
                        CLEAR_SCREEN();
                        printf("No hay espacio para mas eventos\n\n");
                    }
                    setState(MENU);
                    break;

                case (DELETE_EVENT):
                    wfd printEvents();
                    if (events_actived > 0) {
                        wfd deleteEventUI();
                    } else {
                        CLEAR_SCREEN();
                        printf("No hay eventos para eliminar\n\n");
                    }
                    setState(MENU);
                    break;
            }
        }
    }
}

// ---------------------------------- SYSTEM ----------------------------------

void init() {
    events_actived = 0;
    initEvents();
    setState(MENU);
}

void setState(enum STATE newState){
    current_state = newState;
}

enum STATE getState(){
    return current_state;
}

void initEvents(){
    int i;
    struct Event event;

    for (i = 0; i < MAX_NUMBER_EVENTS; i++){
        event.array_postion = -1;
        events[i] = event;
    }
}

// ---------------------------------- MENU ----------------------------------

void menuUI() {
    printf("Menu: \n\n1 - Mostrar hora \n\n2 - Cambiar fecha y hora \n\n3 - Ver eventos \n\n4 - Agregar evento \n\n5 - Eliminar evento \n\n");
	printf("Seleccione una opcion: ");
}

cofunc void optionSelected(){
    char data[4];
    int option;

    waitfor(getswf(data));

    option = converter(data);

    switch (option) {
        case 1:
        		CLEAR_SCREEN();
            setState(DISPLAY_HOUR);
            break;

        case 2:
            CLEAR_SCREEN();
            setState(INPUT_HOUR);
            break;

        case 3:
            setState(LIST_EVENTS);
            break;

        case 4:
            setState(ADD_EVENT);
            break;

        case 5:
            setState(DELETE_EVENT);
            break;

        default:
            CLEAR_SCREEN();
            printf("Por favor seleccione una de las opciones posibles\n\n");
            abort;
    }

}

int converter(char *data) {
    int i, length, result;

    i = 0;
    length = 0;
    result = 0;

    // get the length of the string
    while(*(data+i) != '\0'){
        length++;
        i++;
    }

    i = 0;

    while (*(data+i) != '\0') {

        // get value of string
        *(data + i) = *(data + i) - 0x30;       //0x30 hex = 48 ascii = 0

        // increase depending on the position of the number | 123 = 100*3 + 10*2 + 1*3
        result += multipleOfTen(length - i - 1)*(*(data+i));

        i++;
    }

    return result;
}

int converterBin(char *data) {
    int i, length, result;

    i = 0;
    length = 0;
    result = 0;

    // get the length of the string
    while(*(data+i) != '\0'){
        length++;
        i++;
    }

    i = 0;

    while (*(data+i) != '\0') {

        // get value of string
        *(data + i) = *(data + i) - 0x30;       //0x30 hex = 48 ascii = 0

        // increase depending on the position of the number | 0101 = 0*8 + 1*4 + 0*2 + 1*1
        result += multipleOfTwo(length - i - 1)*(*(data+i));

        i++;
    }

    return result;
}

int multipleOfTen(int multiple) {
    int i, result;

    result = 1;

    for (i = 0; i < multiple; i++) {
        result *= 10;
    }

    return result;
}

int multipleOfTwo(int multiple) {
    int i, result;

    result = 1;

    for (i = 0; i < multiple; i++) {
        result *= 2;
    }

    return result;
}

// ---------------------------------- DATE AND TIME ----------------------------------

unsigned long getRtcTime() {
    // read rtc in seconds from 1 Jan 1980
    // The date and time can be accessed more quickly by reading SEC_TIMER than by reading the RTC
    return SEC_TIMER;
}

void displayHourUI(unsigned long time_in_sec) {
    struct tm time;
    struct tm* time_pointer;

    time_pointer = &time;

    // convert to time structure
    mktm(time_pointer, time_in_sec);

    printf ("%02d/%02d/%04d %02d:%02d:%02d\n\n",
        time.tm_mday, time.tm_mon, 1900 + time.tm_year,
        time.tm_hour, time.tm_min, time.tm_sec);
}

void inputHourUI() {
    displayHourUI(getRtcTime());
    printf("Cambiar fecha y hora \n\n1 - Cambiar hora \n\n2 - Cambiar fecha \n\n3 - Cambiar fecha y hora \n\n4 - Volver \n\n");
    printf("Seleccione una opcion: ");
}

cofunc void setDate() {
    unsigned long time_in_sec;
    struct tm time, *time_pointer;
    char data[4];

    time_pointer = &time;

    waitfor(getswf(data));

    converter(data);

    switch(data[0]) {
        case 1:
            wfd askTimeHourData(&time_in_sec, time_pointer, 1, 0);
            break;

        case 2:
            wfd askTimeHourData(&time_in_sec, time_pointer, 0, 1);
            break;

        case 3:
            wfd askTimeHourData(&time_in_sec, time_pointer, 1, 1);
            break;

        case 4:
        	CLEAR_SCREEN();
        	setState(MENU);
            abort;
            break;

        default:
            CLEAR_SCREEN();
            printf("Por favor seleccione una de las opciones posibles\n\n");
            abort;
    }

    setClock(&time_in_sec, time_pointer);
}

cofunc void askTimeHourData(unsigned long *time_in_sec, struct tm *time_pointer, int ask_time, int ask_date) {

    printf("\n");

    if (ask_time) {
        wfd getTime(time_pointer);
    }
    if (ask_date) {
        wfd getDate(time_pointer);
    }

    *time_in_sec = mktime(time_pointer);
}

cofunc void getTime(struct tm *time_pointer) {
    char hour[10];
	char min[10];
	char sec[10];
    int hour_int, min_int, sec_int, validate;

    validate = 1;

    // Ask for time
    printf("Ingrese hora: ");
    waitfor(getswf(hour));
    printf("Ingrese minutos: ");
    waitfor(getswf(min));
    printf("Ingrese segundos: ");
    waitfor(getswf(sec));

    // Convert strings to int
    hour_int = converter(hour);
    min_int = converter(min);
    sec_int = converter(sec);

    if (validateTime(hour_int, min_int, sec_int)) {
        (*time_pointer).tm_hour = hour_int;
        (*time_pointer).tm_min = min_int;
        (*time_pointer).tm_sec = sec_int;
    } else {
        CLEAR_SCREEN();
        printf ("Datos de hora invalidos\n\n");
        abort;
    }
}

int validateTime(int hour_int, int min_int, int sec_int) {
    int validate;

    validate = 1;

    if (hour_int < 0 | hour_int > 23) {
        validate = 0;
    }
    if (min_int < 0 | min_int > 59) {
        validate = 0;
    }
    if (sec_int < 0 | sec_int > 59) {
        validate = 0;
    }

    return validate;
}

cofunc void getDate(struct tm *time_pointer) {
    char day[10];
	char month[10];
	char year[10];
    int day_int, month_int, year_int, validate;

    validate = 1;

    // Ask for date
    printf("Ingrese dia: ");
    waitfor(getswf(day));
    printf("Ingrese mes: ");
    waitfor(getswf(month));
    printf("Ingrese anio: ");
    waitfor(getswf(year));

    // Convert strings to int
    day_int = converter(day);
    month_int = converter(month);
    year_int = converter(year);

    if (validateDate(day_int, month_int, year_int)) {
        (*time_pointer).tm_mday = day_int;
        (*time_pointer).tm_mon = month_int;
        (*time_pointer).tm_year = year_int - 1900;
    } else {
            CLEAR_SCREEN();
        printf ("Datos de fecha invalidos\n\n");
        abort;
    }
}

int validateDate(int day_int, int month_int, int year_int) {
    int validate;

    validate = 1;

    if (day_int < 0 | day_int > 31) {
            validate = 0;
        }
        if (month_int < 0 | month_int > 12) {
            validate = 0;
        }
        if (year_int < 1900) {
            validate = 0;
        }

    return validate;
}

void setClock(unsigned long *time_in_sec, struct tm *time_pointer) {

    // Set the RTC time
    write_rtc(time_in_sec);


    *time_in_sec = mktime(time_pointer);

    // Update MS_TIMER and SEC_TIMER
    MS_TIMER = (*time_in_sec)*1000;         // Is in miliseconds
    SEC_TIMER = (*time_in_sec);
    CLEAR_SCREEN();
}

// ---------------------------------- EVENT ----------------------------------

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

void printEventData(char data[]) {
    int i;

    i = 0;

    while((data[i]) != '\0'){
        printf("%c", data[i]);
        i++;
    }    
}

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

cofunc void getEventName(char name[]) {

    printf("Por favor ingrese un nombre para el evento (maximo 10 caracteres) : ");

    // get name
    waitfor(getswf(name));
}

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

cofunc void getEventLeds(char* leds) {

    printf("Por favor ingrese una cadena de largo 8 de 0 o 1 para la salida de los leds: \n\n");

    waitfor(getswf(leds));
}

// Given a char print it as binary
void printCharInbin(char data) {
    char a, i;

    a = data;
    for (i = 0; i < 8; i++) {
        printf("%d", !!((a << i) & 0x80));
    }
    printf("\n\n");
}

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

    for (i = 0; i < MAX_NUMBER_EVENTS; i++) {
        if (i == option) {
            events[i].array_postion = -1;
            events_actived -= 1;
        }
    }

    CLEAR_SCREEN();
    printf("Se elimino correctamente el evento\n\n");
}
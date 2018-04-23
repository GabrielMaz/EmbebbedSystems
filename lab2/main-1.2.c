#define CLEAR_SCREEN() (printf(" \x1Bt"));

enum ACTIONS {
    SET_LED_STATUS
};

enum STATE {
    MENU = 0,
    DISPLAY_HOUR = 1,
    INPUT_HOUR = 2,
    LIST_EVENTS = 3,
    DELETE_EVENT = 4,
    ADD_EVENT = 5
};

typedef struct Event {
    int id;
    char command;
    char param;
    unsigned long time;
};

int current_state;
int MAX_NUMBER_EVENTS;
struct Event events[5];

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
cofunc void inputHourUI();
cofunc void askTimeHourData(int ask_time, int ask_date);
cofunc void askForDateTime(unsigned long* time_in_sec);

// ---------------------------------- EVENT ----------------------------------
cofunc void handleCommandInput(int* command_int);
cofunc void handleParamInput(int* param_int);
cofunc void createEvent(int command, int params, unsigned long time);
cofunc void createEventUi();
cofunc void printEvents();
void insertEvent(struct Event event);

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

                case (DELETE_EVENT):
                    setState(MENU);
                    break;

                case (ADD_EVENT):
                    wfd createEventUi();
                    setState(MENU);
                    break;
            }
        }
    }
}

// ---------------------------------- SYSTEM ----------------------------------

void initEvents(){
    int i;
    struct Event event;

    for (i = 0; i < MAX_NUMBER_EVENTS; i++){
        event.id = -1;
        events[i] = event;
    }
}

void setState(enum STATE newState){
    current_state = newState;
}

enum STATE getState(){
    return current_state;
}

void init() {
    MAX_NUMBER_EVENTS = 5;
    initEvents();
    setState(MENU);
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
            printf("DELETE_EVENT\n");
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

    char data[4];

    waitfor(getswf(data));

    converter(data);

    switch(data[0]) {
        case 1:
            wfd askTimeHourData(1,0);
            break;

        case 2:
            wfd askTimeHourData(0,1);
            break;

        case 3:
            wfd askTimeHourData(1,1);
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
}

cofunc void askTimeHourData(int ask_time, int ask_date) {
    struct tm time;
    struct tm *time_pointer;
    unsigned long time_in_sec;
    char day[10];
	char month[10];
	char year[10];
    char hour[10];
	char min[10];
	char sec[10];
    int day_int;
    int month_int;
    int year_int;
    int hour_int;
    int min_int;
    int sec_int;
    int validate;

    time_pointer = &time;

    printf("\n");

    if (ask_date) {
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

        if (day_int < 0 | day_int > 31) {
            validate = 0;
        }
        if (month_int < 0 | month_int > 12) {
            validate = 0;
        }
        if (year_int < 1900) {
            validate = 0;
        }

        if (validate) {
            time.tm_mday = day_int;
            time.tm_mon = month_int;
            time.tm_year = year_int - 1900;
        } else {
        	   CLEAR_SCREEN();
            printf ("Datos de fecha invalidos\n\n");
            abort;
        }
    }

    if (ask_time) {
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

        if (hour_int < 0 | hour_int > 23) {
            validate = 0;
        }
        if (min_int < 0 | min_int > 59) {
            validate = 0;
        }
        if (sec_int < 0 | sec_int > 59) {
            validate = 0;
        }

        if (validate) {
            time.tm_hour = hour_int;
            time.tm_min = min_int;
            time.tm_sec = sec_int;
        } else {
        	CLEAR_SCREEN();
            printf ("Datos de hora invalidos\n\n");
            abort;
        }
    }

    time_in_sec = mktime(time_pointer);

    // Set the RTC time
    write_rtc(time_in_sec);

    // Update MS_TIMER and SEC_TIMER
    MS_TIMER = time_in_sec*1000;         // Is in miliseconds
    SEC_TIMER = time_in_sec;
    CLEAR_SCREEN();
}

cofunc void askForDateTime(unsigned long* time_in_sec){
    struct tm time;
    struct tm *time_pointer;
    char day[10];
	 char month[10];
	 char year[10];
    char hour[10];
	 char min[10];
	 char sec[10];
    int day_int;
    int month_int;
    int year_int;
    int hour_int;
    int min_int;
    int sec_int;
    int validate;

    time_pointer = &time;

    printf("\n");

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

    if (day_int < 0 | day_int > 31) {
        validate = 0;
    }
    if (month_int < 0 | month_int > 12) {
        validate = 0;
    }
    if (year_int < 1900) {
        validate = 0;
    }

    if (validate) {
        time.tm_mday = day_int;
        time.tm_mon = month_int;
        time.tm_year = year_int - 1900;
    } else {
            CLEAR_SCREEN();
        printf ("Datos de fecha invalidos\n\n");
        abort;
    }

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

    if (hour_int < 0 | hour_int > 23) {
        validate = 0;
    }
    if (min_int < 0 | min_int > 59) {
        validate = 0;
    }
    if (sec_int < 0 | sec_int > 59) {
        validate = 0;
    }

    if (validate) {
        time.tm_hour = hour_int;
        time.tm_min = min_int;
        time.tm_sec = sec_int;
    } else {
        CLEAR_SCREEN();
        printf ("Datos de hora invalidos\n\n");
        abort;
    }

    *time_in_sec = mktime(time_pointer);
}

// ---------------------------------- EVENT ----------------------------------

cofunc void handleCommandInput(int* command_int){
    char command[4];

    printf("Por favor seleccione uno de los siguientes comandos:\n\n");
    printf("0 - Prender leds\n\n");
    // add more actions here if needed

    printf("Seleccione una opcion: ");

    // get command, should be only 8 bits plus EOS
    waitfor(getswf(command));
    *command_int = converter(command);
}

cofunc void handleParamInput(int* param_int){
    char param[12];

    // print message for user ux
    printf("Por favor ingrese el parametro a ser usado en la ejecucion del comando:\n\n");
    printf("-- Debe ser una cadena de 0 o 1 de largo maximo 8 --\n\n");

    waitfor(getswf(param));

    *param_int = converterBin(param);
}

cofunc void createEvent(int command, int params, unsigned long time){
    struct Event event;

    event.command = command;
    event.param = params;
    event.time = time;
    insertEvent(event);
}

cofunc void createEventUi(){
    int command;
    int params;
    unsigned long time;

    CLEAR_SCREEN();
    wfd handleCommandInput(&command);
    // check if entered value belongs to the possible values
    if (!(command == 0)) {
         // if not in options, abort
        CLEAR_SCREEN();
        printf("Error: por favor ingrese un commando valido\n\n");
        abort;
    }
    CLEAR_SCREEN();
    wfd handleParamInput(&params);
    // validate if only 1 and o
    CLEAR_SCREEN();
    wfd askForDateTime(&time);
    CLEAR_SCREEN();
    printf("Creating event....");
    wfd createEvent(command, params, time);
    printf("OK");
    yield;
    CLEAR_SCREEN();
    setState(MENU);
}

// Given a char print it as binary
void printCharInbin(char data){
    char a;
    int i;

    a = data;
    for (i = 0; i < 8; i++) {
        printf("%d", !!((a << i) & 0x80));
    }
    printf("\n\n");
}


cofunc void printEvents(){
    struct Event event;
    int i;

    CLEAR_SCREEN();

    printf("Listado de eventos:\n\n");

    for (i = 0; i < MAX_NUMBER_EVENTS; i++){
        event = events[i];

        if (event.id != -1) {
            printf("Comando de evento: Prender leds\n\n");
            printf("Parametro de evento: ");
            printCharInbin(event.param);
            displayHourUI(event.time);
            printf("==============<>==============\n\n");
        }

    }

    printf("\n\n");
}

void insertEvent(struct Event event) {
    int i, inserted;

    inserted = 0;

    for (i = 0; i < MAX_NUMBER_EVENTS; i++) {
        if (events[i].id == -1) {
            event.id = i;
            events[i] = event;
            inserted = 1;
            break;
        }
    }

    if (!inserted) {
        printf("No hay espacio para mas eventos");
    }
}
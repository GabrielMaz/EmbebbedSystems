/*** BeginHeader getRtcTime */
unsigned long getRtcTime();
/*** EndHeader */

unsigned long getRtcTime() {
    // read rtc in seconds from 1 Jan 1980
    // The date and time can be accessed more quickly by reading SEC_TIMER than by reading the RTC
    return SEC_TIMER;
}

/*** BeginHeader displayHourUI */
void displayHourUI(unsigned long time_in_sec);
/*** EndHeader */

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

/*** BeginHeader inputHourUI */
void inputHourUI();
/*** EndHeader */

void inputHourUI() {
    displayHourUI(getRtcTime());
    printf("Cambiar fecha y hora \n\n1 - Cambiar hora \n\n2 - Cambiar fecha \n\n3 - Cambiar fecha y hora \n\n4 - Volver \n\n");
    printf("Seleccione una opcion: ");
}

/*** BeginHeader setDate */
cofunc void setDate();
/*** EndHeader */

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

/*** BeginHeader askTimeHourData */
cofunc void askTimeHourData(unsigned long *time_in_sec, struct tm *time_pointer, int ask_time, int ask_date);
/*** EndHeader */

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

/*** BeginHeader getTime */
cofunc void getTime(struct tm *time_pointer);
/*** EndHeader */

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

/*** BeginHeader validateTime */
int validateTime(int hour_int, int min_int, int sec_int);
/*** EndHeader */

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

/*** BeginHeader getDate */
cofunc void getDate(struct tm *time_pointer);
/*** EndHeader */

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

/*** BeginHeader validateDate */
int validateDate(int day_int, int month_int, int year_int);
/*** EndHeader */

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

/*** BeginHeader setClock */
void setClock(unsigned long *time_in_sec, struct tm *time_pointer);
/*** EndHeader */

void setClock(unsigned long *time_in_sec, struct tm *time_pointer) {

    // Set the RTC time
    write_rtc(time_in_sec);


    *time_in_sec = mktime(time_pointer);

    // Update MS_TIMER and SEC_TIMER
    MS_TIMER = (*time_in_sec)*1000;         // Is in miliseconds
    SEC_TIMER = (*time_in_sec);
    CLEAR_SCREEN();
}
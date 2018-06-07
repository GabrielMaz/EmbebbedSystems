/*** BeginHeader */

#use LAB5_SYSTEM.LIB
#use LAB5_UCOS.LIB
#use LAB5_GPS_Custom.LIB

/*** EndHeader */

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
    char result[22];
    
    time_pointer = &time;

    // convert to time structure
    mktm(time_pointer, time_in_sec);

    sprintf (result, "%02d/%02d/%04d %02d:%02d:%02d\n",
        time.tm_mday, time.tm_mon, 1900 + time.tm_year,
        time.tm_hour, time.tm_min, time.tm_sec);

    printEthernet(result);

}

/*** BeginHeader inputHourUI */
void inputHourUI();
/*** EndHeader */

void inputHourUI() {
    displayHourUI(getRtcTime());

    printEthernet("Cambiar fecha y hora \n\n1 - Cambiar hora \n\n2 - Cambiar fecha \n\n3 - Cambiar fecha y hora \n\n4 - Volver \n\nSeleccione una opcion: ");    
}

/*** BeginHeader askTimeHourDataEthernet */
int askTimeHourDataEthernet(unsigned long *time_in_sec, struct tm *time_pointer, int ask_time, int ask_date, int *time_validate, int *date_validate);
/*** EndHeader */

int askTimeHourDataEthernet(unsigned long *time_in_sec, struct tm *time_pointer, int ask_time, int ask_date, int *time_validate, int *date_validate) {

    if (ask_time) {
        while(!getTimeEthernet(time_pointer, time_validate)) { DELAY100MS(); }
    }

    if (ask_date & *time_validate) {
        while(!getDateEthernet(time_pointer, date_validate)) { DELAY100MS(); }
    }
    
    if (*time_validate & *date_validate) {
        *time_in_sec = mktime(time_pointer);
    
    } else {
        return -1;
    }

    return 1;
}

/*** BeginHeader getTimeEthernet */
int getTimeEthernet(struct tm *time_pointer, int *time_validate);
/*** EndHeader */

int getTimeEthernet(struct tm *time_pointer, int *time_validate) {
    char hour[3];
	char min[3];
	char sec[3];
    int hour_int, min_int, sec_int, validate;

    CLEAR_BUFFER();

    printEthernet("\nIngrese hora: ");
    while(!sock_gets(&socket, hour, 3)) { ucosDelay(0, 0, 0, 100); }
    CLEAR_SOCKET();
    printEthernet("\nIngrese minutos: ");
    while(!sock_gets(&socket, min, 3)) { ucosDelay(0, 0, 0, 100); }
    CLEAR_SOCKET();
    printEthernet("\nIngrese segundos: ");
    while(!sock_gets(&socket, sec, 3)) { ucosDelay(0, 0, 0, 100); }
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

    // Convert strings to int
    hour_int = converter(hour);
    min_int = converter(min);
    sec_int = converter(sec);

    if (validateTime(hour_int, min_int, sec_int)) {
        (*time_pointer).tm_hour = hour_int;
        (*time_pointer).tm_min = min_int;
        (*time_pointer).tm_sec = sec_int;

    } else {
        clearScreenEthernet();
        printEthernet("Datos de hora invalidos\n");
        *time_validate = 0;
    }

    return 1;
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

/*** BeginHeader getDateEthernet */
int getDateEthernet(struct tm *time_pointer, int *date_validate);
/*** EndHeader */

int getDateEthernet(struct tm *time_pointer, int *date_validate) {
    char day[3];
	char month[3];
	char year[5];
    int day_int, month_int, year_int, validate;

    // Get date info
    printEthernet("\nIngrese dia: ");
    while(!sock_gets(&socket, day, 3)) { ucosDelay(0, 0, 0, 100); }
    CLEAR_SOCKET();
    printEthernet("\nIngrese mes: ");
    while(!sock_gets(&socket, month, 3)) { ucosDelay(0, 0, 0, 100); }
    CLEAR_SOCKET();
    printEthernet("\nIngrese anio: ");
    while(!sock_gets(&socket, year, 5)) { ucosDelay(0, 0, 0, 100); }
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

    // Convert strings to int
    day_int = converter(day);
    month_int = converter(month);
    year_int = converter(year);

    // Validate the info
    if (validateDate(day_int, month_int, year_int)) {

        (*time_pointer).tm_mday = day_int;
        (*time_pointer).tm_mon = month_int;
        (*time_pointer).tm_year = year_int - 1900;

    } else {
        clearScreenEthernet();
        printEthernet("Datos de fecha invalidos\n");
        *date_validate = 0;
    }

    return 1;
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
    write_rtc(*time_in_sec);

    *time_in_sec = mktime(time_pointer);

    // Update MS_TIMER and SEC_TIMER
    MS_TIMER = (*time_in_sec)*1000;         // Is in miliseconds
    SEC_TIMER = (*time_in_sec);

    clearScreenEthernet();
}

/*** BeginHeader needUpdate */
void needUpdate();
/*** EndHeader */
void needUpdate() {
    struct tm time, *time_pointer, utc, *utc_pointer;
    char trama[60];
    unsigned long time_in_sec, utm_in_sec;
    
    time_pointer = &time;
    utc_pointer = &utc;

    GPS_gets(trama);

    printf("%s", trama);

    getTramaTime(utc_pointer, trama);

    utm_in_sec = mktime(utc_pointer);

    time_in_sec = getRtcTime();

    // convert to tm structure
    mktm(time_pointer, time_in_sec);

    if (gps_get_utc(utc_pointer, trama)) {
        convertTimeZone(utc_pointer, &utm_in_sec);
        
        if (utc.tm_year == time.tm_year &
            utc.tm_mon == time.tm_mon &
            utc.tm_mday == time.tm_mday &
            utc.tm_hour == time.tm_hour &
            utc.tm_min == time.tm_min) {

            if (utc.tm_sec > time.tm_sec) {
                if (utc.tm_sec - time.tm_sec < 15) {
                    setClock(&utm_in_sec, utc_pointer);
                }

            } else {
                if (time.tm_sec - utc.tm_sec < 15) {
                    setClock(&utm_in_sec, utc_pointer);
                }
            }
        }
    }
}

/*** BeginHeader getTramaTime */
void getTramaTime(struct tm *uct_pointer, char *trama);
/*** EndHeader */

void getTramaTime(struct tm *uct_pointer, char *trama) {
    char data[2];

    // Hour
    data[0] = *(trama+7);
    data[1] = *(trama+8);
    (*uct_pointer).tm_hour = converter(data);

    // Min
    data[0] = *(trama+9);
    data[1] = *(trama+10);
    (*uct_pointer).tm_min = converter(data);

    // Sec
    data[0] = *(trama+11);
    data[1] = *(trama+12);
    (*uct_pointer).tm_sec = converter(data);

    // Day
    data[0] = *(trama+51);
    data[1] = *(trama+52);
    (*uct_pointer).tm_mday = converter(data);

    // Month
    data[0] = *(trama+53);
    data[1] = *(trama+54);
    (*uct_pointer).tm_mon = converter(data);

    // Year
    // (data)10 + 2000 = 2010 the actual year
    // in struct tm save actual year - 1900
    // so data + 100
    data[0] = *(trama+55);
    data[1] = *(trama+56);
    (*uct_pointer).tm_year = converter(data) + 100;               
}

/*** BeginHeader convertTimeZone */
void convertTimeZone(struct tm *utc_pointer, unsigned long *utm_in_sec);
/*** EndHeader */

 void convertTimeZone(struct tm *utc_pointer, unsigned long *utm_in_sec) {
    *utm_in_sec = *utm_in_sec - 10800L;                  // 3600 sec * 3 = 10800 = 3 hours

    mktm(utc_pointer, *utm_in_sec);
}
/*** BeginHeader */

#use LAB4_SYSTEM.LIB
#use LAB4_UCOS.LIB

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
void displayHourUI(unsigned long time_in_sec, int console);
/*** EndHeader */

void displayHourUI(unsigned long time_in_sec, int console) {
    struct tm time;
    struct tm* time_pointer;
    char aux[6], result[22];
    
    time_pointer = &time;

    // convert to time structure
    mktm(time_pointer, time_in_sec);

    if (console) {
        printf ("%02d/%02d/%04d %02d:%02d:%02d\n\n",
            time.tm_mday, time.tm_mon, 1900 + time.tm_year,
            time.tm_hour, time.tm_min, time.tm_sec);
    } else {

        sprintf (result, "%02d/%02d/%04d %02d:%02d:%02d\n",
            time.tm_mday, time.tm_mon, 1900 + time.tm_year,
            time.tm_hour, time.tm_min, time.tm_sec);

        printEthernet(result);
    }
}

/*** BeginHeader inputHourUI */
void inputHourUI(int console);
/*** EndHeader */

void inputHourUI(int console) {
    displayHourUI(getRtcTime(), console);

    if (console) {
        printf("Cambiar fecha y hora \n\n1 - Cambiar hora \n\n2 - Cambiar fecha \n\n3 - Cambiar fecha y hora \n\n4 - Volver \n\n");
        printf("Seleccione una opcion: ");
    } else {
        printEthernet("Cambiar fecha y hora \n\n1 - Cambiar hora \n\n2 - Cambiar fecha \n\n3 - Cambiar fecha y hora \n\n4 - Volver \n\nSeleccione una opcion: ");
    }
    
}

/*** BeginHeader setDate */
void setDate(int console);
/*** EndHeader */

void setDate(int console) {
    unsigned long time_in_sec;
    struct tm time, *time_pointer;
    char data[4];
    int option, time_validated, date_validate;

    time_pointer = &time;

    time_validated = 1;
    date_validate = 1;

    if (console) {
        while(!getswf(data)) { ucosDelay(0, 0, 0, 100); }
        option = converter(data);
    
    } else {
        while(!sock_gets(&socket, data, 4)) { ucosDelay(0, 0, 0, 100); }
        option = converter(data);

        sock_err:
        switch(status)
        {
            case 1: /* foreign host closed */
                printf("User closed session\n");
                break;
            case -1: /* time-out */
                printf("Connection timed out\n");
                break;
        }
    }

    switch(option) {
        case 1:
            if (console) {
                while(!askTimeHourData(&time_in_sec, time_pointer, 1, 0, &time_validated, &date_validate)) { 
                    if (askTimeHourData(&time_in_sec, time_pointer, 1, 0, &time_validated, &date_validate) == -1) {
                        return;
                    }
                    DELAY100MS(); 
                }

            } else {
                while(!askTimeHourDataEthernet(&time_in_sec, time_pointer, 1, 0, &time_validated, &date_validate)) { 
                    if (askTimeHourDataEthernet(&time_in_sec, time_pointer, 1, 0, &time_validated, &date_validate) == -1) {
                        return;
                    }
                    DELAY100MS(); 
                }
            }
            break;

        case 2:
            if (console) {
                while(!askTimeHourData(&time_in_sec, time_pointer, 0, 1, &time_validated, &date_validate)) { 
                    if (askTimeHourData(&time_in_sec, time_pointer, 0, 1, &time_validated, &date_validate) == -1) {
                        return;
                    }
                    DELAY100MS();
                }

            } else {
                while(!askTimeHourDataEthernet(&time_in_sec, time_pointer, 0, 1, &time_validated, &date_validate)) { 
                    if (askTimeHourDataEthernet(&time_in_sec, time_pointer, 0, 1, &time_validated, &date_validate) == -1) {
                        return;
                    }
                    DELAY100MS(); 
                }
            }
            break;

        case 3:
            if (console) {
                while(!askTimeHourData(&time_in_sec, time_pointer, 1, 1, &time_validated, &date_validate)) { 
                    if (askTimeHourData(&time_in_sec, time_pointer, 1, 1, &time_validated, &date_validate) == -1) {
                        return;
                    }
                    DELAY100MS(); 
                }

            } else {
                while(!askTimeHourDataEthernet(&time_in_sec, time_pointer, 1, 1, &time_validated, &date_validate)) { 
                    if (askTimeHourDataEthernet(&time_in_sec, time_pointer, 1, 1, &time_validated, &date_validate) == -1) {
                        return;
                    }
                    DELAY100MS(); 
                }
            }
            break;

        case 4:
            if (console) {
        	    CLEAR_SCREEN();
            } else {
                clearScreenEthernet();
            }

        	setState(INITIAL);
            return;

        default:
            if (console) {
                CLEAR_SCREEN();
                printf("Por favor seleccione una de las opciones posibles\n\n");
            } else {
                tcp_tick(&socket);
                clearScreenEthernet();
                printEthernet("Por favor seleccione una de las opciones posibles\n\n");
            }
            return;
    }

    if (!(time_validated & date_validate)) {
        return;
    }

    setClock(&time_in_sec, time_pointer, console);
}

/*** BeginHeader askTimeHourData */
int askTimeHourData(unsigned long *time_in_sec, struct tm *time_pointer, int ask_time, int ask_date, int *time_validate, int *date_validate);
/*** EndHeader */

int askTimeHourData(unsigned long *time_in_sec, struct tm *time_pointer, int ask_time, int ask_date, int *time_validate, int *date_validate) {

    printf("\n");

    if (ask_time) {
        while(!getTime(time_pointer, time_validate)) { DELAY100MS(); }
    }

    if (ask_date & *time_validate) {
        while(!getDate(time_pointer, date_validate)) { DELAY100MS(); }
    }

    if (*time_validate & *date_validate) {
        *time_in_sec = mktime(time_pointer);

    } else {
        return -1;
    }
    
    return 1;
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

/*** BeginHeader getTime */
int getTime(struct tm *time_pointer, int *time_validate);
/*** EndHeader */

int getTime(struct tm *time_pointer, int *time_validate) {
    char hour[10];
	char min[10];
	char sec[10];
    int hour_int, min_int, sec_int;

    // Ask for time
    printf("Ingrese hora: ");
    while(!getswf(hour)) { ucosDelay(0, 0, 0, 100); }
    printf("Ingrese minutos: ");
    while(!getswf(min)) { ucosDelay(0, 0, 0, 100); }
    ("Ingrese segundos: ");
    while(!getswf(sec)) { ucosDelay(0, 0, 0, 100); }

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
        *time_validate = 0;
    }
    return 1;
}

/*** BeginHeader getTimeEthernet */
int getTimeEthernet(struct tm *time_pointer, int *time_validate);
/*** EndHeader */

int getTimeEthernet(struct tm *time_pointer, int *time_validate) {
    char hour[10];
	char min[10];
	char sec[10];
    int hour_int, min_int, sec_int, validate;

    CLEAR_BUFFER();

    printEthernet("\nIngrese hora: ");
    while(!sock_gets(&socket, hour, 10)) { ucosDelay(0, 0, 0, 100); }
    printEthernet("\nIngrese minutos: ");
    while(!sock_gets(&socket, min, 10)) { ucosDelay(0, 0, 0, 100); }
    printEthernet("\nIngrese segundos: ");
    while(!sock_gets(&socket, sec, 10)) { ucosDelay(0, 0, 0, 100); }

    // Convert strings to int
    hour_int = converter(hour);
    min_int = converter(min);
    sec_int = converter(sec);
    
    sock_err:
    switch(status) {
        case 1: /* foreign host closed */
            printf("User closed session\n");
            break;
        case -1: /* time-out */
            printf("Connection timed out\n");
            break;
    }

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

/*** BeginHeader getDate */
int getDate(struct tm *time_pointer, int *date_validate);
/*** EndHeader */

int getDate(struct tm *time_pointer, int *date_validate) {
    char day[10];
	char month[10];
	char year[10];
    int day_int, month_int, year_int;

    // Ask for date
    printf("Ingrese dia: ");
    while(!getswf(day)) { ucosDelay(0, 0, 0, 100); }
    printf("Ingrese mes: ");
    while(!getswf(month)) { ucosDelay(0, 0, 0, 100); }
    printf("Ingrese anio: ");
    while(!getswf(year)) { ucosDelay(0, 0, 0, 100); }

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
        *date_validate = 0;
    }

    return 1;
}

/*** BeginHeader getDateEthernet */
int getDateEthernet(struct tm *time_pointer, int *date_validate);
/*** EndHeader */

int getDateEthernet(struct tm *time_pointer, int *date_validate) {
    char day[10];
	char month[10];
	char year[10];
    int day_int, month_int, year_int, validate;

    // Get date info
    printEthernet("\nIngrese dia: ");
    while(!sock_gets(&socket, day, 10)) { ucosDelay(0, 0, 0, 100); }
    printEthernet("\nIngrese mes: ");
    while(!sock_gets(&socket, month, 10)) { ucosDelay(0, 0, 0, 100); }
    printEthernet("\nIngrese anio: ");
    while(!sock_gets(&socket, year, 10)) { ucosDelay(0, 0, 0, 100); }

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
void setClock(unsigned long *time_in_sec, struct tm *time_pointer, int console);
/*** EndHeader */

void setClock(unsigned long *time_in_sec, struct tm *time_pointer, int console) {

    // Set the RTC time
    write_rtc(*time_in_sec);

    *time_in_sec = mktime(time_pointer);

    // Update MS_TIMER and SEC_TIMER
    MS_TIMER = (*time_in_sec)*1000;         // Is in miliseconds
    SEC_TIMER = (*time_in_sec);

    if (console) {
        CLEAR_SCREEN();
    } else {
        clearScreenEthernet();
    }
}
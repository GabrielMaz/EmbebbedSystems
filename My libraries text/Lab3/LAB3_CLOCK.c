/*** BeginHeader */

#use LAB3_SYSTEM.LIB

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
cofunc void setDate(int console);
/*** EndHeader */

cofunc void setDate(int console) {
    unsigned long time_in_sec;
    struct tm time, *time_pointer;
    char data[4];
    int option, time_validated, date_validate;

    time_pointer = &time;

    time_validated = 1;
    date_validate = 1;

    if (console) {
        waitfor(getswf(data));
        option = converter(data);
    } else {
        while (tcp_tick(&socket)) {
            if(sock_gets(&socket,buffer_msg,250)) {
                option = converter(buffer_msg);
                CLEAR_BUFFER();
                break;
            }
            waitfor(DelayMs(100));
        }
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
                wfd askTimeHourData(&time_in_sec, time_pointer, 1, 0);
            } else {
                wfd askTimeHourDataEthernet(&time_in_sec, time_pointer, 1, 0, &time_validated, &date_validate);
            }
            break;

        case 2:
            if (console) {
                wfd askTimeHourData(&time_in_sec, time_pointer, 0, 1);
            } else {
                wfd askTimeHourDataEthernet(&time_in_sec, time_pointer, 0, 1, &time_validated, &date_validate);
            }
            break;

        case 3:
            if (console) {
                wfd askTimeHourData(&time_in_sec, time_pointer, 1, 1);
            } else {
                wfd askTimeHourDataEthernet(&time_in_sec, time_pointer, 1, 1, &time_validated, &date_validate);
            }
            break;

        case 4:
            if (console) {
        	    CLEAR_SCREEN();
            } else {
                tcp_tick(&socket);
                clearScreenEthernet();
            }

        	setState(INITIAL);
            abort;
            break;

        default:
            if (console) {
                CLEAR_SCREEN();
                printf("Por favor seleccione una de las opciones posibles\n\n");
            } else {
                tcp_tick(&socket);
                clearScreenEthernet();
                printEthernet("Por favor seleccione una de las opciones posibles\n\n");
            }
            abort;
    }

    if (!(time_validated & date_validate)) {
        abort;
    }

    setClock(&time_in_sec, time_pointer, console);
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

/*** BeginHeader askTimeHourDataEthernet */
cofunc void askTimeHourDataEthernet(unsigned long *time_in_sec, struct tm *time_pointer, int ask_time, int ask_date, int *time_validate, int *date_validate);
/*** EndHeader */

cofunc void askTimeHourDataEthernet(unsigned long *time_in_sec, struct tm *time_pointer, int ask_time, int ask_date, int *time_validate, int *date_validate) {

    if (ask_time) {
        wfd getTimeEthernet(time_pointer, time_validate);
    }
    if (ask_date & *time_validate) {
        wfd getDateEthernet(time_pointer, date_validate);
    }

    if (*time_validate & *date_validate) {
        *time_in_sec = mktime(time_pointer);
    }
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

/*** BeginHeader getTimeEthernet */
cofunc void getTimeEthernet(struct tm *time_pointer, int *time_validate);
/*** EndHeader */

cofunc void getTimeEthernet(struct tm *time_pointer, int *time_validate) {
    int hour_int, min_int, sec_int, validate, state, printed, exit_while;

    validate = 1;
    state = 1;
    printed = 0;
    exit_while = 0;

    CLEAR_BUFFER();

    while (tcp_tick(&socket)) {
        switch (state) {
            case 1:
                if (!printed) {
                    printEthernet("\nIngrese hora: ");
                    printed = 1;
                }

                if(sock_gets(&socket,buffer_msg,250)) {
                    hour_int = converter(buffer_msg);
                    CLEAR_BUFFER();
                    state = 2;
                    printed = 0;
                }
            
            case 2:
                if (!printed) {
                    printEthernet("\nIngrese minutos: ");
                    printed = 1;
                }

                if(sock_gets(&socket,buffer_msg,250)) {
                    min_int = converter(buffer_msg);
                    CLEAR_BUFFER();
                    state = 3;
                    printed = 0;
                }

            case 3:
                if (!printed) {
                    printEthernet("\nIngrese segundos: ");
                    printed = 1;
                }
                if(sock_gets(&socket,buffer_msg,250)) {
                    sec_int = converter(buffer_msg);
                    CLEAR_BUFFER();
                    exit_while = 1;
                }
        }
        if (exit_while) {
            break;
        }
        waitfor(DelayMs(100));
    }
    
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

/*** BeginHeader getDateEthernet */
cofunc void getDateEthernet(struct tm *time_pointer, int *date_validate);
/*** EndHeader */

cofunc void getDateEthernet(struct tm *time_pointer, int *date_validate) {
    int day_int, month_int, year_int, validate, state, printed, exit_while;

    validate = 1;
    state = 1;
    printed = 0;
    exit_while = 0;

    while (tcp_tick(&socket)) {
        switch (state) {
            case 1:
                if (!printed) {
                    printEthernet("\nIngrese dia: ");
                    printed = 1;
                }

                if(sock_gets(&socket,buffer_msg,250)) {
                    day_int = converter(buffer_msg);
                    CLEAR_BUFFER();
                    state = 2;
                    printed = 0;
                }
            
            case 2:
                if (!printed) {
                    printEthernet("\nIngrese mes: ");
                    printed = 1;
                }

                if(sock_gets(&socket,buffer_msg,250)) {
                    month_int = converter(buffer_msg);
                    CLEAR_BUFFER();
                    state = 3;
                    printed = 0;
                }

            case 3:
                if (!printed) {
                    printEthernet("\nIngrese anio: ");
                    printed = 1;
                }
                if(sock_gets(&socket,buffer_msg,250)) {
                    year_int = converter(buffer_msg);
                    CLEAR_BUFFER();
                    exit_while = 1;
                }
        }

        if (exit_while) {
            break;
        }
        
        waitfor(DelayMs(100));
    }

    sock_err:
    switch(status) {
        case 1: /* foreign host closed */
            printf("User closed session\n");
            break;
        case -1: /* time-out */
            printf("Connection timed out\n");
            break;
    }

    if (validateDate(day_int, month_int, year_int)) {
        (*time_pointer).tm_mday = day_int;
        (*time_pointer).tm_mon = month_int;
        (*time_pointer).tm_year = year_int - 1900;
    } else {
        clearScreenEthernet();
        printEthernet("Datos de fecha invalidos\n");
        *date_validate = 0;
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
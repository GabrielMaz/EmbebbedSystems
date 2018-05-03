/*** BeginHeader */
#use LAB3_SYSTEM.LIB
#use LAB3_ETHERNET.LIB

#define CLIENTS 50

/*** EndHeader */

/*** BeginHeader menuUI */
void menuUI(int console);
/*** EndHeader */

void menuUI(int console) {
    if (console) {
        printf("Menu: \n\n1 - Mostrar hora \n\n2 - Cambiar fecha y hora \n\n3 - Ver eventos \n\n4 - Agregar evento \n\n5 - Eliminar evento \n\n");
        printf("6 - Mostrar entrada analogica 1 \n\n7 - Mostrar entrada analogica 2\n\n ");
        printf("Seleccione una opcion: ");
    } else {

        printEthernet("Menu: \n\n1 - Mostrar hora \n\n2 - Cambiar fecha y hora \n\n3 - Ver eventos \n\n4 - Agregar evento \n\n5 - Eliminar evento \n\n6 - Mostrar entrada analogica 1 \n\n7 - Mostrar entrada analogica 2 \n\nSeleccione una opcion: ");
    }
}

/*** BeginHeader optionSelected */
cofunc void optionSelected(int console);
/*** EndHeader */

cofunc void optionSelected(int console) {
    char data[2];
    int option;

    if (console) {
        waitfor(getswf(data));
    } else {
        while (tcp_tick(&socket)) {
            sock_wait_input(&socket,0,NULL,&status);
            if(sock_gets(&socket,buffer,2048)) {
                data[0] = buffer[0];
                CLEAR_BUFFER();
                break;
            }
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

    option = converter(data);

    printf ("\n option: %d", option);

    switch (option) {
        case 1:
            if (console) {
                CLEAR_SCREEN();
            } else {
                cleanScreenEthernet();
            }
        	
            setState(DISPLAY_HOUR);
            break;

        case 2:
            if (console) {
                CLEAR_SCREEN();
            } else {
                cleanScreenEthernet();
            }

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

        case 6:
            if (console) {
                CLEAR_SCREEN();
            } else {
                cleanScreenEthernet();
            }
            
            setState(ANALOG_INPUT_0);
            break;

        case 7:
            if (console) {
                CLEAR_SCREEN();
            } else {
                cleanScreenEthernet();
            }

            setState(ANALOG_INPUT_1);
            break;

        default:
            //CLEAR_SCREEN();
            printf("Por favor seleccione una de las opciones posibles\n\n");
            abort;
    }

}

/*** BeginHeader converter */
int converter(char *data);
/*** EndHeader */

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

/*** BeginHeader multipleOfTen */
int multipleOfTen(int multiple);
/*** EndHeader */

int multipleOfTen(int multiple) {
    int i, result;

    result = 1;

    for (i = 0; i < multiple; i++) {
        result *= 10;
    }

    return result;
}

/*** BeginHeader multipleOfTwo */
int multipleOfTwo(int multiple);
/*** EndHeader */

int multipleOfTwo(int multiple) {
    int i, result;

    result = 1;

    for (i = 0; i < multiple; i++) {
        result *= 2;
    }

    return result;
}

/*** BeginHeader selectOption */
cofunc void selectOption(int state, int console);
/*** EndHeader */

cofunc void selectOption(int state, int console) {
    switch(state){
        case MENU:
            menuUI(console);
            wfd optionSelected(console);
            break;

        case DISPLAY_HOUR:
            displayHourUI(getRtcTime());
            setState(MENU);
            break;

        case INPUT_HOUR:
            inputHourUI();
            wfd setDate();
            break;

        case LIST_EVENTS:
            wfd printEvents();
            setState(MENU);
            break;

        case ADD_EVENT:
            if (events_actived < MAX_NUMBER_EVENTS) {
                wfd createEventUi();
            } else {
                CLEAR_SCREEN();
                printf("No hay espacio para mas eventos\n\n");
            }
            setState(MENU);
            break;

        case DELETE_EVENT:
            if (console) {
                if (events_actived > 0) {
                    wfd deleteEventUI();
                } else {
                    CLEAR_SCREEN();
                    printf("No hay eventos para eliminar\n\n");
                }
            } else {
                cleanScreenEthernet();
            }
            wfd printEvents(console);
            
            setState(MENU);
            break;
        
        case ANALOG_INPUT_0:
            getAnalogInput(0);
            delayMS(PIC_TIMEOUT);
            setState(MENU);
            break;

        case ANALOG_INPUT_1:
            getAnalogInput(1);
            delayMS(PIC_TIMEOUT);
            setState(MENU);
            break;
    }
}

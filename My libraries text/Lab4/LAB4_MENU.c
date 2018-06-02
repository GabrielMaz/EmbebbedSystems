/*** BeginHeader */
#use LAB4_SYSTEM.LIB
#use LAB4_ETHERNET.LIB
#use LAB4_UCOS.LIB

#define CLIENTS 50

/*** EndHeader */

/*** BeginHeader menuUI */
void menuUI();
/*** EndHeader */

void menuUI() {
    printEthernet("Menu: \n\n1 - Mostrar hora \n\n2 - Cambiar fecha y hora \n\n3 - Ver eventos \n\n4 - Agregar evento \n\n5 - Eliminar evento \n\n6 - Mostrar entrada analogica 1 \n\n7 - Mostrar entrada analogica 2 \n\nSeleccione una opcion: ");
}

/*** BeginHeader optionSelected */
void optionSelected();
/*** EndHeader */

void optionSelected() {
    char data[4];
    int option;

    while (!(sock_gets(&socket, data, 4))) {
        DELAY100MS();
    }
    
    option = converter(data);

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
    
    switch (option) {
        case 1:
            clearScreenEthernet();
            
            setState(DISPLAY_HOUR);
            break;

        case 2:
            clearScreenEthernet();

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
            clearScreenEthernet();
            
            setState(ANALOG_INPUT_0);
            break;

        case 7:
            clearScreenEthernet();

            setState(ANALOG_INPUT_1);
            break;

        default:
            clearScreenEthernet();
            printEthernet("Por favor seleccione una de las opciones posibles\n\n");
            return;
            
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
        *(data + i) = *(data + i) - 0x30;                       //0x30 hex = 48 ascii = 0

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
void selectOption(int state);
/*** EndHeader */

void selectOption(int state) {
    switch(state){
        case INITIAL:
            menuUI();
            setState(MENU);
            break;

        case MENU:
            optionSelected();
            break;

        case DISPLAY_HOUR:
            displayHourUI(getRtcTime());
            setState(INITIAL);
            break;

        case INPUT_HOUR:
            inputHourUI();
            setDate();
            break;

        case LIST_EVENTS:
            clearScreenEthernet();
            printEventsEthernet();
            
            setState(INITIAL);
            break;

        case ADD_EVENT:
            createEventUi();
            setState(INITIAL);
            break;

        case DELETE_EVENT:
            if (events_actived > 0) {
                clearScreenEthernet();
                printEventsEthernet();
                deleteEventEthernetUI();
            
            } else {
                clearScreenEthernet();
                printEthernet("No hay eventos para eliminar\n");
            }
            clearScreenEthernet();

            setState(INITIAL);
            
            break;
        
        case ANALOG_INPUT_0:
            getAnalogInput(0);
            DELAY_MS(PIC_TIMEOUT);
            setState(INITIAL);
            break;

        case ANALOG_INPUT_1:
            getAnalogInput(1);
            DELAY_MS(PIC_TIMEOUT);
            setState(INITIAL);
            break;
    }
}

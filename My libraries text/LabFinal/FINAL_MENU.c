/*** BeginHeader */
#use FINAL_SYSTEM.LIB
#use FINAL_ETHERNET.LIB
#use FINAL_UCOS.LIB
#use FINAL_GPRS.LIB

#define CLIENTS 50

/*** EndHeader */

/*** BeginHeader menuUI */
void menuUI();
/*** EndHeader */

void menuUI() {
    printEthernet("Menu: \n\n1 - Ver contactos \n\n2 - Agregar contacto \n\n3 - Eliminar contacto \n\n4 - Ubicacion del vehiculo\n\nSeleccione una opcion:");    
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

    switch (option) {
        case 1:
            setState(LIST_CONTACTS);
            break;

        case 2:
            setState(ADD_CONTACT);
            break;

        case 3:
            setState(DELETE_CONTACT);
            break;

        case 4:
            setState(LOCATION);
            break;

        case 5: 
            setState(DATA_SAVED); 
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
    auto INT8U err;
    switch(state){
        case INITIAL:
            menuUI();
            setState(MENU);
            break;

        case MENU:
            optionSelected();
            break;

        case LIST_CONTACTS:
            clearScreenEthernet();
            DELAY100MS();
            printAgendaEthernet();
            setState(INITIAL);
            break;

        case ADD_CONTACT:
            addContact();
            setState(INITIAL);
            break;

        case DELETE_CONTACT:
            clearScreenEthernet();
            printAgendaEthernet();
            deleteContactEthernet();
            clearScreenEthernet();

            setState(INITIAL);
            break;

        case DATA_SAVED: 
            //sendMessage(); 
            printEthernet("Finish\n"); 
            setState(INITIAL); 
            break; 

        case LOCATION:

            break;
    }
}

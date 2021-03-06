/*** BeginHeader menuUI */
void menuUI();
/*** EndHeader */

void menuUI() {
    printf("Menu: \n\n1 - Mostrar hora \n\n2 - Cambiar fecha y hora \n\n3 - Ver eventos \n\n4 - Agregar evento \n\n5 - Eliminar evento \n\n");
	printf("Seleccione una opcion: ");
}

/*** BeginHeader optionSelected */
cofunc void optionSelected();
/*** EndHeader */

cofunc void optionSelected() {
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
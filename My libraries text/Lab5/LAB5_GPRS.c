/*** BeginHeader */
#use LAB5_SYSTEM.LIB
#use LAB5_UCOS.LIB

/*** EndHeader */

/*** BeginHeader modemReady */
int modemReady();
/*** EndHeader */

int modemReady() {
    while(1) {
        DELAY100MS();

        BitWrPortI(PEDDR, &PEDDRShadow, PORT_OUTPUT, 4);
        setOutput(PORT_E, GPRS_PWKEY, 0);

        // delay > 1 s
        DELAY_S_MS(1, 200);

        BitWrPortI(PEDDR, &PEDDRShadow, PORT_INPUT, 4);

        // delay > 2 s
        DELAY_S_MS(2, 200);

        if (!getInput(PORT_E, GPRS_STATUS)) {
            printf("No se prendio el modem");
            continue;
        }
        printf("Se prendio el modem");
        return 1;
    }
}

/*** BeginHeader sendMessage */
void sendMessage(char *message);
/*** EndHeader */

void sendMessage(char *message) {
    serDputs(message);
    serDputc('\n');
}

/*** BeginHeader readMessage */
void readMessage(char *result);
/*** EndHeader */

void readMessage(char *result) {
    char result[100];
    int index;

    character = serDget();
    index = 0;

    while (1) {
        if (character = -1) {
            continue;

        } else {
            (*result)[index++] = character;
        }
    }
}
/*** BeginHeader */
#use FINAL_SYSTEM.LIB
#use FINAL_UCOS.LIB

#define GPRS_PWKEY                      4
#define GPRS_STATUS                     1
#define MESSAGE_ACCIDENT                "CHOCO EL VEHICULO"

/*** EndHeader */

/*** BeginHeader modemReady */
int modemReady();
/*** EndHeader */

int modemReady() {
    int limit_time;

    limit_time = 0;

    while(limit_time < 3) {
        DELAY100MS();

        BitWrPortI(PEDDR, &PEDDRShadow, PORT_OUTPUT, 4);
        setOutput(PORT_E, GPRS_PWKEY, 0);

        // delay > 1 s
        DELAY_S_MS(1, 100);

        BitWrPortI(PEDDR, &PEDDRShadow, PORT_INPUT, 4);

        // delay > 2 s
        DELAY_S_MS(2, 100);

        
        //if (!BitRdPortI(PORT_E, GPRS_STATUS)) {
        if (!getInput(PORT_E, GPRS_STATUS)) {
            printf("Se prendio el modem");
            return 1;
        }

        limit_time++;
    }

    printf("No se prendio el modem");
}

/*** BeginHeader synchronizeRabbit */
int synchronizeRabbit();
/*** EndHeader */

int synchronizeRabbit() {
    char response[2];

    serDputc('A');
    DELAY_S_MS(5, 0);
    serDputs("AT");
    
    // Wait for the response
    while(!serDread(response, 2, 500));

    // Check if response is OK
    if (response[0] == 'O' & response[1] == 'K') {
        printf("\nSe sincronizo el rabbit");
        return 1;
    }
    printf("\nNo se sincronizo el rabbit");
    return 0;
}

/*** BeginHeader sendMessage */
void sendMessage(char *message);
/*** EndHeader */

void sendMessage(char *message) {
    serDputs(message);
    serDputc('\n');
}

/*** BeginHeader readMessage */
void readMessage(char *result, int lenght);
/*** EndHeader */

void readMessage(char *result) {
    serDread(result, lenght, );
}

/*** BeginHeader send */
void send();
/*** EndHeader */

void send() {
    char name[11];
    char phone[9];
    char msg[15];
    char result[50];

    askName(name);
    askPhone(phone);
    askMsg(msg);

    sprintf(result, "AT+CMGS=\"");
    strcat(result, phone);
    strcat(result, "\"");
    serDputs(result);

    serDputc(0x0D);

    serDputs(msg);

    serDputc(0x1A);
}

/*** BeginHeader sendAllContacts */
void sendAllContacts();
/*** EndHeader */

void sendAllContacts() {
    int i;
    char result[50];

    for (i=0; i<5; i++) {
        if (contacts[i].array_postion != -1) {

            sprintf(result, "AT+CMGS=\"");
            strcat(result, contacts[i].phone);
            strcat(result, "\"");
            serDputs(result);

            serDputc(0x0D);

            serDputs(MESSAGE_ACCIDENT);

            serDputc(0x1A);
        }
    }
}
/*** BeginHeader */
#use FINAL_SYSTEM.LIB
#use FINAL_UCOS.LIB

#define GPRS_PWKEY                      4
#define GPRS_STATUS                     1
#define MESSAGE_ACCIDENT                "CHOCO EL VEHICULO EN LA SIGUIENTE DIRECCION "

/*** EndHeader */

/*** BeginHeader modemReady */
void modemReady();
/*** EndHeader */

void modemReady() {
    int limit_time;

    limit_time = 0;

    printf("\n Modem:\n");

    if (getInput(PORT_E, GPRS_STATUS)) { 
            printf("\tSe prendio el modem\n"); 
            return; 
    } else { 

        while(limit_time < 3) {
            printf("\tIntento: %d\n", limit_time);
            DELAY100MS();

            BitWrPortI(PEDDR, &PEDDRShadow, PORT_OUTPUT, 4);
            setOutput(PORT_E, GPRS_PWKEY, 0);

            // delay > 1 s
            DELAY_S_MS(1, 100);

            BitWrPortI(PEDDR, &PEDDRShadow, PORT_INPUT, 4);

            // delay > 2 s
            DELAY_S_MS(2, 100);

            
            if (getInput(PORT_E, GPRS_STATUS)) {
                printf("\tSe prendio el modem\n");
                return;
            }

            limit_time++;
            printf("\tNo se prendio el modem\n");
        }
    }    
}

/*** BeginHeader synchronizeRabbit */
int synchronizeRabbit();
/*** EndHeader */

int synchronizeRabbit() {
    char response[8];

    serDputc('A');
    DELAY_S_MS(5, 0);
    serDputs("AT\r");
    
    // Wait for the response
    while(!serDread(response, 8, 500));

    // Check if response is OK
    if (strstr(response, "OK") != NULL) {
        printf("\tSe sincronizo\n\n");
        return 1;
    }
    printf("\tSincronizando...\n");
    return 0;
}

/*** BeginHeader checkNewMessage */
int checkNewMessage();
/*** EndHeader */

int checkNewMessage() {
    char response[256];
    int i;
    
    serDputs("AT+CMGL=\"REC UNREAD\"\r");
    
    memset(response,0,256);
    while(!serDread(response, 256, 500));
    return (strstr(response, "+CMGL:") != NULL);
}

/*** BeginHeader send */
void send(char *phone, char *msg);
/*** EndHeader */

void send(char *phone, char *msg) {
    char result[20];                        // AT+CMGS=\"094560289\" = 19 char + end of string

    sprintf(result, "AT+CMGS=\"");
    strcat(result, phone);
    strcat(result, "\"");
    serDputs(result);

    serDputc(0x0D);

    serDputs(msg);

    serDputc(0x1A);
}

/*** BeginHeader askPhone */
void askPhone(char *phone);
/*** EndHeader */

void askPhone(char *phone) {
    printEthernet("\nPor favor ingrese un numero de destino (9 digitos): ");
    CLEAR_BUFFER();

    // Wait for input
    while(!sock_gets(&socket, phone, 11)) { DELAY100MS(); }
    
    CLEAR_SOCKET();
}

/*** BeginHeader askMsg */
void askMsg(char *msg);
/*** EndHeader */

void askMsg(char *msg) {
    printEthernet("\nPor favor ingrese su mensaje (maximo 15 caracteres): ");
    CLEAR_BUFFER();

    // Wait for input
    while(!sock_gets(&socket, msg, 11)) { DELAY100MS(); }
    
    CLEAR_SOCKET();
}

/*** BeginHeader alertAllContacts */
void alertAllContacts(char *link);
/*** EndHeader */

void alertAllContacts(char *link) {
    int i;
    char result[50], msg[95];

    for (i=0; i<5; i++) {
        if (contacts[i].array_postion != -1) {

            sprintf(result, "AT+CMGS=\"");
            strcat(result, contacts[i].phone);
            strcat(result, "\"");
            serDputs(result);

            serDputc(0x0D);

            strcat(msg, MESSAGE_ACCIDENT);
            strcat(msg, link);
            serDputs(msg);

            serDputc(0x1A);
        }
    }
}
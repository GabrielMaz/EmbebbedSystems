/*** BeginHeader */
#use FINAL_SYSTEM.LIB
#use FINAL_UCOS.LIB

#define GPRS_PWKEY                      4
#define GPRS_STATUS                     1
#define MESSAGE_ACCIDENT                "CHOCO EL VEHICULO"
#define CHOQUE                          "CHOCO VEHICULO"
#define POSICION                        "POSICION VEHICULO"
#define PHONE                           "+598"

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
void checkNewMessage();
/*** EndHeader */

void checkNewMessage() {
    char celular[9], response[256], *phone_pointer, * mem_block;
    int i;
    
    serDputs("AT+CMGL=\"REC UNREAD\"\r");

    serDrdFlush();
    
    // Clean response
    memset(response, 0, 256);

    // Clean celular
    memset(celular, '\0', 9);

    // Wait for read
    while(!serDread(response, 256, 500));

    if (strstr(response, POSICION) != NULL) {

        phone_pointer = strstr(response, PHONE);

        if (phone_pointer != NULL){
            getPhoneFromMsg(phone_pointer, celular);
            if (friendlyPhone(celular)) {
                mem_block = (char*) OSMemGet(mem_pointer, &err); 
                strcpy(mem_block, celular); 
                OSQPost(queue_pointer, mem_block);
                
                // Celan the box to avoid full box
                cleanBox();
            }
        }
    }
}

/*** BeginHeader sendMessage */
void sendMessage(char *phone, char *msg);
/*** EndHeader */

void sendMessage(char *phone, char *msg) {
    char result[30], response[35];
    int i;

    printf("\n Enviando mensaje\n");

    serDrdFlush();

    sprintf(result, "AT+CMGS=\"");
    strcat(result, phone);
    strcat(result, "\"");
    serDputs(result);

    serDputc(0x0D);

    while(!serDread(response, 20, 500));

    serDputs(msg);

    serDputc(0x1A);    

    while(!serDread(response, 35, 500));
}

/*** BeginHeader alertAllContacts */
void alertAllContacts();
/*** EndHeader */

void alertAllContacts() {
    struct Contact contact, *contact_pointer;
    char result[80];
    int i;
    unsigned int addr;

    contact_pointer = &contact;

    // Initial position to userBlock
    addr = 0;

    for (i = 0; i < MAX_NUMBER_CONTACTS; i++){
        // Get the contact
        readUserBlock(contact_pointer, addr, BLOCK_SIZE);

        // If not empty, send message
        if (contact.array_position != -1) {

            sendMessage(contact.phone, MESSAGE_ACCIDENT);
        }

        // Increase addr to find the next block
        addr += BLOCK_SIZE;
    }
}

/*** BeginHeader getPhoneFromMsg */
void getPhoneFromMsg(char *response, char *result);
 /*** EndHeader */

void getPhoneFromMsg(char *response, char *result) {
     int i;

    // First digit allways 0
    *(result) = '0';

    // First digit allways 9
    *(result+1) = '9';

    // Get the other 7 digits
     for (i = 2; i<10; i++) {
         *(result+i) = *(response+i+3);
     }
 }

 /*** BeginHeader cleanBox */
 void cleanBox();
 /*** EndHeader */
  
  void cleanBox() {
    char response[256];
    int i;

    serDputs("AT+CMGDA=\"DEL ALL\"\r");
    serDrdFlush();
    memset(response,0,256);
    while(!serDread(response, 256, 500));

    printf("\n\n");
}
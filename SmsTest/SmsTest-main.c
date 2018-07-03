enum portName {
    PORT_A = PADR,
    PORT_E = PEDR
};

enum bitNumber {
    GPRS_STATUS = 1,
	GPRS_PWKEY = 4,
	RED_LED = 5
};

#define PORT_INPUT 0
#define PORT_OUTPUT 1

char* getPortShadow(enum portName p_port) {
    switch(p_port){
        case PORT_A:
            return &PADRShadow;

        case PORT_E:
            return &PEDRShadow;
    }
}

void setOutput(enum portName p_port, enum bitNumber p_pin, unsigned char p_state){
    BitWrPortI(p_port, getPortShadow(p_port), p_state, p_pin);
}

char getInput(enum portName p_port, enum bitNumber p_pin) {
	return BitRdPortI(p_port, p_pin);
}

void configurePorts(){
    // setup port A as output
    WrPortI(SPCR, &SPCRShadow, 0x84);

    // set port A with all zeros
    WrPortI(PADR, &PADRShadow, 0x00);

    // setup port E as output (red led)
    BitWrPortI(PEDDR, &PEDDRShadow, PORT_OUTPUT, 5);

	// set pin 1 port E as input (GPRS_STATUS)
	BitWrPortI(PEDDR, &PEDDRShadow, PORT_INPUT, 1);

	// set pin 4 port E as input (GPRS_PWKEY)
	BitWrPortI(PEDDR, &PEDDRShadow, PORT_INPUT, 4);
    
	// serial port C
	serCopen(9600);

	// serial port D
	serDopen(9600);
}

void delay(int ms_delay){
    auto unsigned long t0;

    // ms timer is updated by rabbit
    t0 = MS_TIMER;

    while (MS_TIMER < (t0 + ms_delay));
}

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
            delay(100);

            BitWrPortI(PEDDR, &PEDDRShadow, PORT_OUTPUT, 4);
            setOutput(PORT_E, GPRS_PWKEY, 0);

            // delay > 1 s
            delay(1100);

            BitWrPortI(PEDDR, &PEDDRShadow, PORT_INPUT, 4);

            // delay > 2 s
           delay(2100);

            if (getInput(PORT_E, GPRS_STATUS)) {
                printf("\tSe prendio el modem\n");
                return;
            }

            limit_time++;
            printf("\tNo se prendio el modem\n");
        }
    }
}

int synchronizeRabbit() {
    char response[8];
    int i;

    serDputc('A');
    delay(5000);
    serDputs("AT\r");
    
    // Wait for the response
    while(!serDread(response, 8, 500));

    // Check if response is OK
    if (strstr(response, "OK") != NULL) {
        for (i=0; i<8; i++) {
        printf("%d - %c\n", i, response[i]);
    }
        printf("\tSe sincronizo el Rabbit\n\n");
        return 1;
    }
    printf("\tSincronizando el Rabbit\n");
    return 0;
}

void send(char * data) {
    char response[44];
    int i;
    serDputs("AT+CMGS=\"094560289\"");

    while(!serDread(response, 20, 500));

    for (i=0; i<44; i++) {
        printf("%d - %c\n", i, response[i]);
    }

    serDputc(0x0D);

    serDputs(data);

    serDputc(0x1A);

    while(!serDread(response, 44, 500));

    for (i=0; i<17; i++) {
        printf("%d - %c\n", i, response[i]);
    }
}

void read(char *data) {
    char response[50];
    int i;
    serDputs("AT+CMGL=0");
    
    while(!serDread(response, 50, 500));
    for (i=0; i<50; i++) {
        printf("%d - %c \n", i, response[i]);
    }
}

void main (void) {
    char data[20];
    configurePorts();
    modemReady();
    while(!synchronizeRabbit()) { delay(100); }

    while (1) {
        printf("Mensaje: ");
        while(!getswf(data));
        //send(data);
        read(data);
    }
}
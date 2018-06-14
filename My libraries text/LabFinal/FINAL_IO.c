/*** BeginHeader */

#define PORT_INPUT 0
#define PORT_OUTPUT 1

enum portName {
    PORT_A = PADR,
    PORT_E = PEDR
};

enum bitNumber {
    GPRS_STATUS = 1,
	GPRS_PWKEY = 4,
	RED_LED = 5
};

/*** EndHeader */

/*** BeginHeader configurePorts */
void configurePorts();
/*** EndHeader */

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
	serCopen(PIC_BAUDRATE);

	// serial port D
	serDopen(PIC_BAUDRATE);
}

/*** BeginHeader getPortShadow */
char* getPortShadow(enum portName p_port);
/*** EndHeader */

char* getPortShadow(enum portName p_port) {
    switch(p_port){
        case PORT_A:
            return &PADRShadow;

        case PORT_E:
            return &PEDRShadow;
    }
}

/*** BeginHeader setOutput */
void setOutput(enum portName p_port, enum bitNumber p_pin, unsigned char p_state);
/*** EndHeader */

void setOutput(enum portName p_port, enum bitNumber p_pin, unsigned char p_state){
    BitWrPortI(p_port, getPortShadow(p_port), p_state, p_pin);
}

/*** BeginHeader getInput */
 char getInput(enum portName p_port, enum bitNumber p_pin);
 /*** EndHeader */

char getInput(enum portName p_port, enum bitNumber p_pin) {
	return BitRdPortI(p_port, p_pin);
}

/*** BeginHeader IO_getAnalogInput */
int IO_getAnalogInput( unsigned char p_input );
/*** EndHeader */

int IO_getAnalogInput( unsigned char p_input )
{
	char datos[PIC_MAX_MSG_LENGTH];
	char crc, i;
	int retVal;
	unsigned long timeout;

	if( p_input > 1 )
	{
		return -1; // Solamente hay entradas 0 y 1
	}

	serCrdFlush();
	serCwrFlush();

	serCputc(STX);
	serCputc('0');
	serCputc('1');
	serCputc(ETX);
	serCputc(STX + ETX + '1' + '0');

	timeout = MS_TIMER + PIC_COM_TIMEOUT;
	while( (serCrdUsed() != PIC_MAX_MSG_LENGTH) && ((long)(MS_TIMER-timeout) < 0) );
	if( serCrdUsed() != PIC_MAX_MSG_LENGTH )
	{
		return -2;
	}

	serCread( datos, PIC_MAX_MSG_LENGTH, 500 );

	crc = 0;
	for( i=0; i<PIC_MAX_MSG_LENGTH-1; i++ )
	{
		crc += datos[i];
	}
	if( crc != datos[PIC_MAX_MSG_LENGTH-1] )
	{
		return -3;
	}

	if (datos[0] != STX || datos[1] != '0' || datos[2] != '1' || datos[27] != ETX)
    {
		return -4;
    }

	retVal = UT_ASCIItoCharNum(datos[p_input * 3 + 3]) * 256;
	retVal += UT_ASCIItoCharNum(datos[p_input * 3 + 4]) * 16;
	retVal += UT_ASCIItoCharNum(datos[p_input * 3 + 5]);

	return retVal;
}
#define INPUT					0
#define OUTPUT					1

#define STX 					0x02
#define ETX 					0x03

#define BTN_GET(x)				getInput( (x>3)?PORT_F:PORT_B, (x>3)?x:(x+2) )
#define BTN_IsPRESSED(x)		((BTN_GET(x)==0)?1:0)

#define PIC_MAX_LENGHT_MSG		29
#define PIC_BAUDRATE			9600
#define PIC_TIMEOUT				100

enum portName
{
	PORT_UNDEF = -1,
	PORT_A,
	PORT_B,
	PORT_C,
	PORT_D,
	PORT_E,
	PORT_F
};

enum bitNumber
{
	BIT_UNDEF = -1,
	BIT_0,
	BIT_1,
	BIT_2,
	BIT_3,
	BIT_4,
	BIT_5,
	BIT_6,
	BIT_7
};

void Delay( long p_ms )
{
	long timer;

	timer = MS_TIMER + p_ms;

	while ((long)(MS_TIMER - timer) < 0);
}

char converterASCIITochar(char p_input) {
	if (p_input >= '0' && p_input <= '9') {
		return (p_input-'0');
   }

   	if (p_input >= 'A' && p_input <= 'F') {
		return (p_input + 10 - 'A');
   }

	return -1;
}

void HW_init( void )
{
	// PORT B
	BitWrPortI(PBDDR, &PBDDRShadow, INPUT, BIT_2); 	//Entrada 0
	BitWrPortI(PBDDR, &PBDDRShadow, INPUT, BIT_3); 	//Entrada 1

	// Puerto Serie PIC
	serCopen(PIC_BAUDRATE);
}

char getInput(enum portName p_port, enum bitNumber p_pin) {
	enum portName port;

	switch( p_port )
	{
		case( PORT_A ):
			port = PADR;
			break;
		case( PORT_B ):
			port = PBDR;
			break;
		case( PORT_C ):
			port = PCDR;
			break;
		case( PORT_D ):
			port = PDDR;
			break;
		case( PORT_E ):
			port = PEDR;
			break;
		case( PORT_F ):
			port = PFDR;
			break;
	}

	return (char)BitRdPortI( port, p_pin );
}

int getAnalogInput( unsigned char analog_input ) {
	char data[PIC_MAX_LENGHT_MSG];
	char crc, i;
	int result;
	unsigned long timeout;

	// analog_input = 0 or 1
	if( analog_input > 1 ) {
		return -1; 
	}

	serCrdFlush();
	serCwrFlush();

	serCputc(STX);
	serCputc('0');
	serCputc('1');
	serCputc(ETX);
	serCputc(STX + ETX + '1' + '0');

	timeout = MS_TIMER + PIC_TIMEOUT;

	while((serCrdUsed() != PIC_MAX_LENGHT_MSG) && ((long)(MS_TIMER-timeout) < 0));

	if(serCrdUsed() != PIC_MAX_LENGHT_MSG) {
		return -2;
	}

	serCread(data, PIC_MAX_LENGHT_MSG, 500);

	crc = 0;
	for(i=0; i<PIC_MAX_LENGHT_MSG-1; i++)	{
		crc += data[i];
	}

	// check crc
	if(crc != data[PIC_MAX_LENGHT_MSG-1]) {
		return -3;
	}

	// check pic format
	if (data[0] != STX || data[1] != '0' || data[2] != '1' || data[27] != ETX) {
		return -4;
   	}

	result = converterASCIITochar(data[analog_input * 3 + 3]) * 256;
	result += converterASCIITochar(data[analog_input * 3 + 4]) * 16;
	result += converterASCIITochar(data[analog_input * 3 + 5]);

	return result;
}


main()
{
	HW_init();

	while(1)
	{
		if( BTN_IsPRESSED(0) )
		{
			printf("Input_0 = %d\n", getAnalogInput( 0 ));
			Delay(1000);
		}
		else if( BTN_IsPRESSED(1) )
		{
			printf("Input_1 = %d\n", getAnalogInput( 1 ));
			Delay(1000);
		}
	}
}
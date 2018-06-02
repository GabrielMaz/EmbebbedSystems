/*** BeginHeader */

#define STX 					0x02
#define ETX 					0x03
#define PIC_MAX_LENGHT_MSG		29
#define PIC_BAUDRATE			9600
#define PIC_TIMEOUT				100
#define CINBUFSIZE 				127
#define COUTBUFSIZE 			127

/*** EndHeader */

/*** BeginHeader converterASCIITochar  */
char converterASCIITochar(char input);
/*** EndHeader */

char converterASCIITochar(char input) {
	if (input >= '0' && input <= '9') {
		return (input-'0');
   }

   	if (input >= 'A' && input <= 'F') {
		return (input + 10 - 'A');
   }

	return -1;
}

/*** BeginHeader getAnalogInput */
void getAnalogInput(unsigned char analog_input);
/*** EndHeader */

void getAnalogInput(unsigned char analog_input) {
	char data[PIC_MAX_LENGHT_MSG];
	char crc, i;
	int result;
	unsigned long timeout;
	char resultEthernet[25];

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
		printf("Ocurrio un error por favor intente de nuevo \n\n");
		return;
	}

	serCread(data, PIC_MAX_LENGHT_MSG, 500);

	crc = 0;
	for(i=0; i<PIC_MAX_LENGHT_MSG-1; i++)	{
		crc += data[i];
	}

	// check crc
	if(crc != data[PIC_MAX_LENGHT_MSG-1]) {
		printf("Ocurrio un error por favor intente de nuevo \n\n");
		return;
	}

	// check pic format
	if (data[0] != STX || data[1] != '0' || data[2] != '1' || data[27] != ETX) {
		printf("Ocurrio un error por favor intente de nuevo \n\n");
		return;
   	}

	result = converterASCIITochar(data[analog_input * 3 + 3]) * 256;
	result += converterASCIITochar(data[analog_input * 3 + 4]) * 16;
	result += converterASCIITochar(data[analog_input * 3 + 5]);

	sprintf(resultEthernet, "Entrada analogica %d = %d", (analog_input+1), result);
	printEthernet(resultEthernet);
}
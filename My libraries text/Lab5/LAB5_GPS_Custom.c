/*** BeginHeader */
#use GPS.lib

#define BINBUFSIZE 		        255
#define BOUTBUFSIZE 		    255
#define SERB_USEPORTD
#define GPS_MAX_BUF_SIZE	    100
#define GPS_CHKSUM_SIZE	        2
#define GPS_CONFIG_DELAY 	    10
#define GPS_VALID_DATA_IDX	    18
#define GPS_VALID_DATA		    'A'
#define GPS_VOID_DATA		    'V'

GPSPosition gpsPosition;
/*** EndHeader */

/*** BeginHeader GPS_init */
void GPS_init( void );
/*** EndHeader */
void GPS_init( void ) {
    BitWrPortI( PDDDR, &PDDDRShadow, 0, 5 );    //Bit 5 como entrada (ARXB)
    BitWrPortI( PDFR, &PDFRShadow, 1, 4 );	    //Bit 4 con funcion alternativa (ATXB)
    serBdatabits( PARAM_8BIT );		            //8 bits de datos
    serBparity( PARAM_NOPARITY ); 		        //Sin paridad
    serBopen( 9600 ); 			                //Baud rate = 9600bps
    serBrdFlush();
    serBwrFlush();

	OSTimeDlyHMSM(0,0,0,100);
	serBputs("$PSRF103,00,00,00,01*24\r\n");	//GGA - off
	OSTimeDlyHMSM(0,0,0,100);
	serBputs("$PSRF103,01,00,00,01*25\r\n");	//GLL - off
	OSTimeDlyHMSM(0,0,0,100);
	serBputs("$PSRF103,02,00,00,01*26\r\n");	//GSA - off
	OSTimeDlyHMSM(0,0,0,100);
	serBputs("$PSRF103,03,00,00,01*27\r\n");	//GSV - off
	OSTimeDlyHMSM(0,0,0,100);
	serBputs("$PSRF103,04,00,01,01*21\r\n");	//RMC - on y enviando la trama cada 1 seg
	OSTimeDlyHMSM(0,0,0,100);
	serBputs("$PSRF103,05,00,00,01*21\r\n");	//VTG - off
	OSTimeDlyHMSM(0,0,0,100);
	serBputs("$PSRF103,08,00,00,01*2C\r\n");	//ZDA - off

  OSTaskDel(OS_PRIO_SELF);
}

/*** BeginHeader GPS_gets */
int GPS_gets( char* p_str );
/*** EndHeader */
int GPS_gets( char* p_str ) {
    char data[GPS_MAX_BUF_SIZE];
    char* start;
    char* end;

    serBrdFlush();

    OSTimeDlyHMSM(0,0,1,500);           // Espero a recibir por lo menos una trama

    if (serBrdUsed() == 0) {
        return -1;
    }

    serBread(data, GPS_MAX_BUF_SIZE, 1);

    start = strchr(data, '$');
    if(start == NULL) {
        return -2;
    }

    end = strchr(start, '*');
    if(end == NULL) {
        return -3;
    }
    *(end+3) = 0;

    //Si la trama es del tipo RMC la leo, sino, la ignoro
    if(memcmp( start, "$GPRMC", strlen("$GPRMC") ) == 0) {	
        strcpy(p_str, start);
        return 1;

    } else {
        return 0;
    }
}
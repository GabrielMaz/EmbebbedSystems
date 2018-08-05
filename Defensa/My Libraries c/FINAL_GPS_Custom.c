/*** BeginHeader */

#define BINBUFSIZE 		        255
#define BOUTBUFSIZE 		    255
#define SERB_USEPORTD
#define GPS_MAX_BUF_SIZE	    100
#define GPS_CHKSUM_SIZE	        2
#define GPS_CONFIG_DELAY 	    10
#define GPS_VALID_DATA_IDX	    18
#define GPS_VALID_DATA		    'A'
#define GPS_VOID_DATA		    'V'
#define LINK                    "​http://maps.google.com/?q="

#use GPS.LIB

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

/*** BeginHeader getPosition */
void getPosition(GPSPosition *position_pointer, char *n_or_s, char *w_or_e);
/*** EndHeader */

void getPosition(GPSPosition *position_pointer, char *n_or_s, char *w_or_e) {
    char trama[85], *position;

    while (!GPS_gets(trama)) {
        OSTimeDlyHMSM (0, 0, 0, 100);
    }

    gps_get_position(position_pointer, trama);

    // Check if the vertical orientation is N or S and save t print it then
    position = strstr(n_or_s, "N");
    if (position != NULL) {
        n_or_s = position;
    } else {
        position = strstr(n_or_s, "S");
        if (position != NULL) {
            n_or_s = position;
        }
    }

    // Check the horizontal orientation
    position = strstr(n_or_s, "W");
    if (position != NULL) {
        w_or_e = position;
    } else {
        position = strstr(n_or_s, "E");
        if (position != NULL) {
            w_or_e = position;
        }
    }

    printf("\n%s\n", trama);
}

/*** BeginHeader generateLinkPosition */
void generateLinkPosition(char *result);
/*** EndHeader */

void generateLinkPosition(char *result) {
    GPSPosition gpsPosition, *gpsPosition_pointer;
    float latitude, longitude;
    char n_or_s, w_or_e;

    printf("Inicio");
    
    gpsPosition_pointer = &gpsPosition;

    printf("1");

    getPosition(gpsPosition_pointer, &n_or_s, &w_or_e);

    printf("2");

    latitude = gpsPosition.lat_degrees + (float) gpsPosition.lat_minutes/60;

    printf("3");

    longitude = gpsPosition.lon_degrees + (float) gpsPosition.lon_minutes/60;

    printf("4");

    sprintf(result, "%s%f%c,%f%c", 
        LINK, 
        latitude, 
        gpsPosition.lat_direction, 
        longitude, 
        gpsPosition.lon_direction);

    printf("5");
}
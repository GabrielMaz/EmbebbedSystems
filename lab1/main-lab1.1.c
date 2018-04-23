#define NUMBER_OF_CHARS_EXPETED 29

void configurePorts();
void sendMessage();
void readSerialPortC(char *data);
int getAnalogInput(unsigned char p_input);
int validateCRC(char *data);
int decodePic(char *data, int analog_input);
char decodeFirstEntry(char input);
char decodeOtherEntry(char input);
int lastEntry(int result, char analog_entry_2);

void main (){
    // configure required port
    configurePorts();

    // get the value of the analog inputs
    getAnalogInput(0);
    getAnalogInput(1);
}

/*
    Configure port c as serial port        
*/
void configurePorts(){
    serCopen(9600);
}

/*
    Sends a message to port C to get the value of the analogic inputs
*/
void sendMessage(){
    // create init message
    char message[6];
    message[0] = 0x02;
    message[1] = 0x30;
    message[2] = 0x31;
    message[3] = 0x03;
    message[4] = 0x66;
    message[5] = '\0'; // end of string
    
    // write message in c port
    serCputs(message);
}

/*
    Reads the serial port c and writes whats there in data
*/
void readSerialPortC(char *data) {
    // used in iterator
    int i;
    unsigned long start;

    // wait for complete message in port c or timeout
    /*start = MS_TIMER;
    while (serCrdUsed() < NUMBER_OF_CHARS_EXPETED + 1) {
        if ((MS_TIMER - start) > 1000) {
            return;
        }
    }*/

    while (serCrdUsed() < NUMBER_OF_CHARS_EXPETED);

    // read chars from port c
    serCread(data, NUMBER_OF_CHARS_EXPETED, 7); //6.042 ms (1/9600 * 2 * 29 * 1000)

    // print data for debugging
    printf("Data read from serial port: ");
    for (i = 0; i < 29; i++) {
        printf("%d/", data[i]);
    }

    printf("\n"); // only for format
}

/*
    Get the value of the given analog input
*/
int getAnalogInput(unsigned char p_input) {
    // define vars
    char data[29]; // char array where result will be stored
    int result; // value of the analog input

    result = -1; // if the data is not valid, then the function will return -1

    printf("input: %d\n", p_input); // print for debugging

    // clean serial port
    serCwrFlush();
    serCrdFlush();

    // request analog data initial message
    sendMessage();

    // read data from serial port
    readSerialPortC(data);

    // if data is valid then return the sum of the data for the input
    if (validateCRC(data)) {
		printf("Data valid");
        // pic returns 3 byts per input, so we return the 3 bytes for the given input
        result = decodePic(data, p_input);
    }

    printf("Result analog input %d: %d\n\n", p_input, result); // print for debugging

    return result;
}

/*
    Check if the data is valid by adding the first 28 values agains the 29
*/
int validateCRC(char *data) {
    // define vars
    long sum;
    int i;

    // init sum accumulator
    sum = 0;

    // summ all values except CRC (index 28)
    for(i = 0; i < 28; i++) {
        sum += data[i];
    }

    // debug prints
    printf("SUM: %d\n", sum);
    printf("CRS result: %d\n", (char) sum);

    // return if 1st 8 bits of sum, equals to the CRC
    return (char) sum == data[28];
}

int decodePic(char *data, int analog_input) {
    char analog_entry_0;
    char analog_entry_1;
    char analog_entry_2;
    int analog_entry_index;
    int result;

    analog_entry_index = (analog_input + 1) * 3;
    analog_entry_0 = decodeFirstEntry(analog_entry_index);
    analog_entry_1 = decodeOtherEntry(analog_entry_index + 1);
    analog_entry_2 = decodeOtherEntry(analog_entry_index + 2);

    result = analog_entry_0 & analog_entry_1;

    return lastEntry(result, analog_entry_2);

}

char decodeFirstEntry(char input) {
    int mask;

    mask = 0x0f;

    return mask | (input << 4);
}

char decodeOtherEntry(char input) {
    int mask;

    mask = 0xf0;

    return mask | input;
}

int lastEntry(int result, char analog_entry_2) {
    int mask;

    mask = 0x000f;

    result = (result  << 4) | mask;

    return result & analog_entry_2;
}
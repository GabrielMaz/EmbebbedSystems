#define TURN_ON_LED(PIN) (setOutput(PORT_A, PIN, 1))
#define TURN_OFF_LED(PIN) (setOutput(PORT_A, PIN, 0))
#define GET_BUTTON_VALUE(BUTTON) (getButtonValue(BUTTON))
#define BUTTON_IS_PRESSED(BUTTON) (getButtonValue(BUTTON) == 0)
#define TIME_DELAY = 500


// possible ports
enum portName {
    PORT_A = PADR,
    PORT_B = PBDR,
    PORT_F = PFDR
};

// PADR pins
enum bitNumber {
    PIN_0 = 0,
    PIN_1 = 1,
    PIN_2 = 2,
    PIN_3 = 3,
    PIN_4 = 4,
    PIN_5 = 5,
    PIN_6 = 6,
    PIN_7 = 7,
};

void configurePorts();
int getPortShadow(enum portName p_port);
void setOutput(enum portName p_port, enum bitNumber p_pin, unsigned char p_state);
char getInput(enum portName p_port, enum bitNumber p_pin);
int getButtonValue(int button);
void delay(int ms_delay);
void ledSequence();
void singleLed (int pin);
enum bitNumber getPin(int button);
void buttonPressedWithLed(int button);
char getLedOutput();

void main (){
    char output;
    configurePorts();

    ledSequence();

    // main loop
    while (1){
        output = getLedOutput();
        WrPortI(PORT_A, getPortShadow(PORT_A), output);
    }
}

void configurePorts(){
    // setup port A as output
    WrPortI(SPCR, &SPCRShadow, 0x84);

    // set port A with all zeros
    WrPortI(PADR, &PADRShadow, 0x00);

    // setup port B as input
    WrPortI(PBDDR, &PBDDRShadow, 0xc3);

    // setup port F as input
    WrPortI(PFDDR, &PFDDRShadow, 0x0f);
}

/*
    Returns the shadows of the ports accepted in portName enum
*/
int getPortShadow(enum portName p_port){
    switch(p_port){
        case PORT_A:
            return &PADRShadow;

        case PORT_B:
            return &PBDRShadow;

        case PORT_F:
            return &PFDRShadow;
    }
}

/*
    Write into the pin of given port the given state
*/
void setOutput(enum portName p_port, enum bitNumber p_pin, unsigned char p_state){
    BitWrPortI(p_port, getPortShadow(p_port), p_state, p_pin);
}

/*
    Return the value of the bit of the port
*/
char getInput(enum portName p_port, enum bitNumber p_pin){
    return BitRdPortI(p_port, p_pin);
}

/*
    delay given ms
*/
void delay(int ms_delay){
    auto unsigned long t0;

    // ms timer is updated by rabbit
    t0 = MS_TIMER;

    while (MS_TIMER < (t0 + ms_delay));
}

/*
    Return 0 if button pressed, 1 if not
*/
int getButtonValue(int button){
    int port;
    int pin;

    if (button < 4){
        port = PORT_B;
        pin = button + 2;
    } else {
        port = PORT_F;
        pin = button;
    }

    return getInput(port, pin);
}

/*
    turn on led, wait 500ms, turn off led and continue with next led
*/
void ledSequence(){
    int i;

    for(i=0; i < 8; i++) {
        singleLed(i);
    }
}

void singleLed (int pin) {
    TURN_ON_LED(getPin(pin));
    delay(500);
    TURN_OFF_LED(getPin(pin));
}

/*
    Return the pin corresponding a given button
*/
enum bitNumber getPin(int button) {
    switch(button) {
        case 0:
            return PIN_0;

        case 1:
            return PIN_1;

        case 2:
            return PIN_2;

        case 3:
            return PIN_3;

        case 4:
            return PIN_4;

        case 5:
            return PIN_5;

        case 6:
            return PIN_6;

        case 7:
            return PIN_7;
    }
}

/*
    Get the values to be set to the leds from the status of the buttons
*/
char getLedOutput() {
    char port_B_values;
    char port_F_values;
    char mask_B;
    char mask_F;

    mask_B = 0xf0;
    mask_F = 0x0f;

    port_B_values = RdPortI(PORT_B);
    port_F_values = RdPortI(PORT_F);
    return ~(((port_B_values >> 2) | mask_B) & (port_F_values | mask_F));
}
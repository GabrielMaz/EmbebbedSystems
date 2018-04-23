#define GET_BUTTON_VALUE(BUTTON) (getButtonValue(BUTTON))
#define BUTTON_IS_PRESSED(BUTTON) (getButtonValue(BUTTON) == 0)

// possible ports
enum portName {
    PORT_B = PBDR,
    PORT_F = PFDR
};

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
void delay1mS(int ms_delay);
int getPortShadow(enum portName p_port);
char getInput(enum portName p_port, enum bitNumber p_pin);
int getButtonValue(int button);

void main (){
    configurePorts();

    // main loop
    while (1){
        delay1mS(100);
        GET_BUTTON_VALUE(0);
    }
}

void configurePorts(){
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
        case PORT_B:
            return &PBDRShadow;
        
        case PORT_F:
            return &PFDRShadow;
    }
}

/*
    Return the value of the bit of the port
*/
char getInput(enum portName p_port, enum bitNumber p_pin){
    return BitRdPortI(p_port, p_pin);
}

/*
*/
void delay1mS(int ms_delay){
    auto unsigned long t0;

    // ms timer is updated by rabbit
    t0 = MS_TIMER;

    while (MS_TIMER < (t0 + ms_delay));
}

/*
    Gets the value of the required button
*/
int getButtonValue(int button){
    int port;
    int pin;

    // map buttons to ports pins
    if (button < 4){
        // buttons from 0 to 3 are assigned to B2-B5
        port = PORT_B;
        pin = button + 2;
    } else {
        // buttons from 4 to 7 are assigned to F4-F7
        port = PORT_F;
        pin = button;
    }
    return getInput(port, pin);
}
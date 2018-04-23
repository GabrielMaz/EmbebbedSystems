#define TURN_ON_LED(PIN) (setOutput(PORT_A, PIN, 1))
#define TURN_OFF_LED(PIN) (setOutput(PORT_A, PIN, 0))


// possible ports
enum portName {
    PORT_A = PADR
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

void main (){
    configurePorts();

    TURN_ON_LED(PIN_0);

    // main loop
    while (1){
        continue;
    }
}

void configurePorts(){
    // setup port A as output
    WrPortI(SPCR, &SPCRShadow, 0x84);

    // set port A with all zeros
    WrPortI(PADR, &PADRShadow, 0x00);
}

/*
    Returns the shadows of the ports accepted in portName enum
*/
int getPortShadow(enum portName p_port){
    switch(p_port){
        case PORT_A:
            return &PADRShadow;
    }
}

/*
    Write into the pin of given port the given state
*/
void setOutput(enum portName p_port, enum bitNumber p_pin, unsigned char p_state){
    BitWrPortI(p_port, getPortShadow(p_port), p_state, p_pin);
}
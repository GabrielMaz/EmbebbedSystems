#define PORT_INPUT 0
#define PORT_OUTPUT 1

// possible ports
enum portName {
    PORT_E = PEDR
};

enum bitNumber {
    BIT_5 = 5
};

void configurePorts();
int getPortShadow(enum portName p_port);
void setOutput(enum portName p_port, enum bitNumber p_pin, unsigned char p_state);


void main (){
    configurePorts();

    // main loop
    while (1){
        costate {
            waitfor(DelayMS(800));
            setOutput(PORT_E, getPortShadow(PORT_E), BIT_5, 1);
            waitfor(DelayMS(400));
            setOutput(PORT_E, getPortShadow(PORT_E), BIT_5, 0);
        }
    }
}

void configurePorts(){
    // setup port E as output
    BitWrPortI(PEDDR, &SPCRShadow, PORT_OUTPUT, 5);
}

/*
    Returns the shadows of the ports accepted in portName enum
*/
int getPortShadow(enum portName p_port){
    switch(p_port){
        case PORT_E:
            return &PEDRShadow;
    }
}

void setOutput(enum portName p_port, enum bitNumber p_pin, unsigned char p_state){
    BitWrPortI(p_port, getPortShadow(p_port), p_state, p_pin);
}
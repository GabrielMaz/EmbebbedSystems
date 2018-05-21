#use LAB3_IO.LIB
#use LAB3_MENU.LIB
#use LAB3_CLOCK.LIB
#use LAB3_EVENT.LIB
#use LAB3_SYSTEM.LIB
#use LAB3_PIC.LIB
#use LAB3_ETHERNET.LIB

void main () {
    char buffer[2048];
	int status;

    initSystem();
    
    initSocket();
    
    current_state = getState();

    while (1) {

        costate {
            tcp_tick(&socket);
        }
        
        // Red led
        costate {
            waitfor(DelayMs(800));
            setOutput(PORT_E, BIT_5, 1);
            waitfor(DelayMs(400));
            setOutput(PORT_E, BIT_5, 0);
        }

        // Console menu
        /*costate {
            wfd selectOption[0](current_state, CONSOLE);
        }*/

        // Ethernet menu
        costate {
            wfd selectOption[1](current_state, ETHERNET);
        }

        // Check for events
        costate {
            checkEventsActivated();
        }
    }
}
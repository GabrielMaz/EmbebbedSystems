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
        tcp_tick(&socket);

        // 1.1
        costate {
            waitfor(DelayMs(800));
            setOutput(PORT_E, BIT_5, 1);
            waitfor(DelayMs(400));
            setOutput(PORT_E, BIT_5, 0);
        }

        // 1.2
        /*costate {
            wfd selectOption(current_state, CONSOLE);
        }*/

        // Ethernet
        costate {
            printf("%d - costate\n", current_state);
            wfd selectOption(current_state, ETHERNET);
        }

        // 1.3
        costate {
            checkEventsActivated();
        }
    }
}
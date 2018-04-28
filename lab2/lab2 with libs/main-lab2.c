#use IO.LIB
#use MENU.LIB
#use CLOCK.LIB
#use EVENT.LIB
#use SYSTEM.LIB

// Main loop function
void main () {
    int input, data_valid;
    unsigned long time;

    init();
    current_state = getState();

    while (1) {
        // 1.1
        costate {
            waitfor(DelayMs(800));
            setOutput(PORT_E, BIT_5, 1);
            waitfor(DelayMs(400));
            setOutput(PORT_E, BIT_5, 0);
        }

        // 1.2
        costate {
            switch(current_state){
                case (MENU):
                    menuUI();
                    wfd optionSelected();
                    break;

                case (DISPLAY_HOUR):
                    displayHourUI(getRtcTime());
                    setState(MENU);
                    break;

                case (INPUT_HOUR):
                    inputHourUI();
                    wfd setDate();
                    break;

                case (LIST_EVENTS):
                    wfd printEvents();
                    setState(MENU);
                    break;

                case (ADD_EVENT):
                    if (events_actived < MAX_NUMBER_EVENTS) {
                        wfd createEventUi();
                    } else {
                        CLEAR_SCREEN();
                        printf("No hay espacio para mas eventos\n\n");
                    }
                    setState(MENU);
                    break;

                case (DELETE_EVENT):
                    wfd printEvents();
                    if (events_actived > 0) {
                        wfd deleteEventUI();
                    } else {
                        CLEAR_SCREEN();
                        printf("No hay eventos para eliminar\n\n");
                    }
                    setState(MENU);
                    break;
            }
        }

        // 1.3
        costate {
            checkEvents();
        }
    }
}
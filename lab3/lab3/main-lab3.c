#use LAB3_IO.LIB
#use LAB3_MENU.LIB
#use LAB3_CLOCK.LIB
#use LAB3_EVENT.LIB
#use LAB3_SYSTEM.LIB
#use LAB3_PIC.LIB

void main () {
    init();

    current_state = getState();

    while (1) {

        costate {
            switch(current_state){
                case MENU:
                    menuUI();
                    wfd optionSelected();
                    break;

                case DISPLAY_HOUR:
                    displayHourUI(getRtcTime());
                    setState(MENU);
                    break;

                case INPUT_HOUR:
                    inputHourUI();
                    wfd setDate();
                    break;

                case LIST_EVENTS:
                    wfd printEvents();
                    setState(MENU);
                    break;

                case ADD_EVENT:
                    if (events_actived < MAX_NUMBER_EVENTS) {
                        wfd createEventUi();
                    } else {
                        CLEAR_SCREEN();
                        printf("No hay espacio para mas eventos\n\n");
                    }
                    setState(MENU);
                    break;

                case DELETE_EVENT:
                    wfd printEvents();
                    if (events_actived > 0) {
                        wfd deleteEventUI();
                    } else {
                        CLEAR_SCREEN();
                        printf("No hay eventos para eliminar\n\n");
                    }
                    setState(MENU);
                    break;
                
                case ANALOG_INPUT_0:
                    CLEAR_SCREEN();
                    getAnalogInput(0);
			        delayMS(PIC_TIMEOUT);
                    setState(MENU);
                    break;

                case ANALOG_INPUT_1:
                    CLEAR_SCREEN();
                    getAnalogInput(1);
			        delayMS(PIC_TIMEOUT);
                    setState(MENU);
                    break;
            }
        }
    }
}
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
	
	printf("Iniciando Socket\n");
	sock_init();
	printf("Iniciado\n");

    init();

    current_state = getState();

    while (1) {

        costate {
        
            wfd selectOption(current_state);
        
        }
    }
}
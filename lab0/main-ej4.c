#include <cstdlib>
#include <stdio.h>

// inputs
char inputs;

// outputs
char outputs;

// states

enum STATES {
    CORRECT_BEHAVIOUR,
    HIGH_TEMP,
    COOLING,
};

// current state
enum STATES current_state;

void setInputs(char input){
    inputs = input;
}

char getInputs(){
    return inputs;
}

void printOutput() {

    // print green led output
    if (0b00001000 & outputs) {
        printf("Green led 1, ");
    } else {
        printf("Green led 0, ");
    }

    // print red led output    
    if (0b00000100 & outputs) {
        printf("Red led 1, ");
    } else {
        printf("Red led 0, ");
    }

    // print alarm output    
    if (0b00000010 & outputs) {
        printf("Alarm 1, ");
    } else {
        printf("Alarm 0, ");
    }

    // print cold control output
    if (0b00000001 & outputs) {
        printf("Cold control 1 \n\n");
    } else {
        printf("Cold control 0 \n\n");
    }
}

void triggerCorrectBehaviour() {
    printf("Correct behavior \n");
    current_state = CORRECT_BEHAVIOUR;

    // 0, 0, 0, 0, green_led, red_led, alarm, cold_control
    outputs = 0b00001000;

    // 0, 0, 0, 0, 0, 0, cool_button, temp_above_max
//    inputs = 0b00000000;

    printOutput();
}

void triggerHighTemp() {
    printf("High temperature \n");
    current_state = HIGH_TEMP;
    outputs = 0b00000110;
    printOutput();
}

void triggerCooling() {
    printf("Cooling \n");
    current_state = COOLING;
    outputs = 0b00000001;
    printOutput();
}

int main(int argc, char **argv) {
        
    // initialize system in correct behavior 
    triggerCorrectBehaviour();
    
    while (1) {
        
        if (!(current_state == COOLING)) {
            printf("Presione: \n 1 por temperatura elevada \n 2 para activar el control de enfriamiento\n");
            inputs = getchar() - 48;
            getchar();
        }
        
        // check states
        switch (current_state) {
                /* 
                  if state is in the correct behavior
                  change in case the temp passes the required 
                  threshold
                 */
            case CORRECT_BEHAVIOUR:
                if (0b00000001 & getInputs()) {
                    triggerHighTemp();
                }
                break;

                /* 
                  if the state is high temp change if
                  the cold button was pressed
                 */
            case HIGH_TEMP:
                if (0b00000010 & getInputs()) {
                    triggerCooling();
                }
                break;

                /* 
                  if the state is cooling move automatically
                  to correct behavior
                 */
            case COOLING:
                triggerCorrectBehaviour();
                break;
        }
    }

    return 0;
}
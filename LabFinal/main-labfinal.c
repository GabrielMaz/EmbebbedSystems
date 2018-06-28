/*******************************************************************************************************

This program does the following:

Start up task:      Creates 5 tasks.
configTask              Check if the modem is on
void creatorTask        Synchronize Rabbit with SIM900
tickTask:			    Excecute ticks to tcp connection.
ethernetTask:		    Excecute program by ethernet.
checkSpeedTask: 	    Check speed.
updateRTCTask.          Update RTC.
getMessages             Check for new messages

********************************************************************************************************/
#class auto 			// Change default storage class for local variables: on the stack*****************************************************************************************************

// Redefine uC/OS-II configuration constants as necessary
#define OS_MAX_EVENTS           6       // Maximum number of events (semaphores + queues + mailboxes + MAX_TCP_SOCKET_BUFFERS + 2)
#define OS_MAX_TASKS            6  		// Maximum number of tasks system can create (less stat and idle tasks)

#define OS_MEM_EN               1
#define OS_TASK_CREATE_EN		1       // Enable normal task creation
#define OS_TASK_CREATE_EXT_EN	0       // Disable extended task creation
#define OS_TASK_DEL_EN          1       // Enable OSTaskDel()
#define OS_TIME_DLY_HMSM_EN	    1		// Enable OSTimeDlyHMSM
#define STACK_CNT_256	        1       // idle task
#define STACK_CNT_512	        8       // number of 512 byte stacks (application tasks + stat task + prog stack)
#define STACK_CNT_2K         	5		// TCP/IP needs a 2K stack
#define MAX_TCP_SOCKET_BUFFERS  1		// One sockets for TCPIP connection.
#define DINBUFSIZE              255
#define DOUTBUFSIZE             255

#use FINAL_SYSTEM.LIB
#use FINAL_IO.LIB
#use FINAL_MENU.LIB
#use FINAL_CLOCK.LIB
#use FINAL_EVENT.LIB
#use FINAL_PIC.LIB
#use FINAL_UCOS.LIB
#use FINAL_ETHERNET.LIB
#use FINAL_GPS_Custom.LIB
#use FINAL_GPRS.LIB
#use FINAL_AGENDA.LIB

/*
*********************************************************************************************************
*                                              CONSTANTS
*********************************************************************************************************
*/

#define          TASK_STK_SIZE     512                /* Size of each task's stacks (# of bytes)       */

#define          TASK_START_ID       0                /* Application tasks IDs                         */
#define          TASK_1_ID           1
#define          TASK_2_ID           2
#define          TASK_3_ID           3
#define          TASK_4_ID           4
#define          TASK_5_ID           5
#define          TASK_6_ID           6
#define          TASK_7_ID           7
#define          TASK_8_ID           8

#define          TASK_START_PRIO    10                /* Application tasks priorities                  */
#define          TASK_1_PRIO        11
#define          TASK_2_PRIO        12
#define          TASK_3_PRIO        13
#define          TASK_4_PRIO        14
#define          TASK_5_PRIO        15
#define          TASK_6_PRIO        16
#define          TASK_7_PRIO        17
#define          TASK_8_PRIO        18

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void configTask(void *data);
void creatorTask(void *data);
void tickTask(void *data);
void ethernetTask(void *data);
void checkSpeedTask(void *data);
void updateRTCTask(void *data);
void getMessages(void *data);

int enter;

/*
*********************************************************************************************************
*                                                  MAIN
*********************************************************************************************************
*/

void main (void) {
    enter = 1;

    printf("\nConfigurando el sistema, por favor espere..\n\n");

    OSInit();
    initSystem();

    clockSem = OSSemCreate(1);

    // GPS_init utiliza delays del sistema operativo, por lo tanto debe ser una tarea.
    // Una vez que se ejecuta se autodestruye.
    OSTaskCreate(GPS_init, NULL, 512, TASK_START_PRIO);

    OSTaskCreate(configTask, NULL, 2048, TASK_1_PRIO);

    OSStart();
}

/*
*********************************************************************************************************
*                                               TASK #1
*
* Description: Wait for modem ready, synchronize rabbit and socket, then config the phone operator.
*********************************************************************************************************
*/

void  configTask (void *data) {
    while (1) {
        if (enter) {
            modemReady();
            printf("\n Rabbit:\n");
            while(!synchronizeRabbit()) { DELAY100MS(); }
            configCops();
            printf("\nListo! Puede comenzar a usar el programa\n\n");
            enter = 0;
            OSTaskCreate(tickTask, NULL, 2048, TASK_2_PRIO);
            OSTaskCreate(ethernetTask, NULL, 2048, TASK_3_PRIO);
            OSTaskCreate(checkSpeedTask, NULL, 2048, TASK_4_PRIO);

        } else {
            DELAY100MS();
        }
    }
}

/*
*********************************************************************************************************
*                                               TASK #3
*
* Description: This task excecute ticks to tcp connection.
*********************************************************************************************************
*/

void tickTask (void *data) {
    while(1) {
        initSocket();
        while( tcp_tick(&socket) )
        {
            OSTimeDlyHMSM(0,0,0,100);                   // Hago tcp_tick 10 veces por segundo
        }
        OSTimeDlyHMSM(0,0,0,100);
    }
}

/*
*********************************************************************************************************
*                                               TASK #4
*
* Description: This task excecute program by ethernet.
*********************************************************************************************************
*/

void  ethernetTask (void *data) {
    while(1) {
        selectOption(current_state);

        OSTimeDly(5);
    }
}

/*
*********************************************************************************************************
*                                               TASK #5
*
* Description: This task check the speed of the vehicle every 10 seconds, if it is over 10g,
                it sends a sms.
*********************************************************************************************************
*/

void  checkSpeedTask (void *data) {
    char link[46];
    int carSpeed;

    while(1) {
        carSpeed = getAnalogInput(0);
        if(carSpeed > 3050) {                               // Crash
            //generateLinkPosition(link);
            setOutput(PORT_E, RED_LED, 1);
            // alertar a todos los contactos
            //alertAllContacts(link);
        } else if (carSpeed > 2500) {                       // Danger
            setOutput(PORT_E, RED_LED, 1);
            OSTimeDlyHMSM(0, 0, 0, 200);
            setOutput(PORT_E, RED_LED, 0);
        } else if (carSpeed > 1500) {                       // Warning
            setOutput(PORT_E, RED_LED, 1);
            OSTimeDlyHMSM(0, 0, 0, 300);
            setOutput(PORT_E, RED_LED, 0);
            OSTimeDlyHMSM(0, 0, 0, 300);
        } else {
            setOutput(PORT_E, RED_LED, 0);                  // Nothing
        }


        /*if (checkSpeed()) {
            //generateLinkPosition(link);
            setOutput(PORT_E, RED_LED, 1);
            printf("Choque");
            // alertar a todos los contactos
            //alertAllContacts(link);
        } else {
            setOutput(PORT_E, RED_LED, 0);
        }*/

        OSTimeDlyHMSM(0, 0, 0, 50);
    }
}

/*
*********************************************************************************************************
*                                               TASK #6
*
* Description: Update the rtc if the diference with GPS is bigger than 15 sec.
*********************************************************************************************************
*/

// probando link position 
void updateRTCTask(void *data) {
    //while(1) {
        //needUpdate();
    //    generateLinkPosition();

    //    OSTimeDlyHMSM(0, 5, 0, 0);
    //}

    char link[46];

    //generateLinkPosition(link);
    //printEthernet(link);
}

/*
*********************************************************************************************************
*                                               TASK #7
*
* Description: Check every 100ms if there are new messages.
*********************************************************************************************************
*/

void getMessages(void *data) {
    char msg[250];

    while (1) {
        if (readMessage(msg) > 0) {
            printf("Nuevo mensaje");
        }
        
        OSTimeDlyHMSM(0, 0, 0, 100);
    }    
}
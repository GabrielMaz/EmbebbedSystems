/*******************************************************************************************************

This program does the following:

Start up task:  Creates 5 tasks.
reciveTask:     	Print in console the info recived from modem.
sendTask:			Sent to modem the info write in console.
					

********************************************************************************************************/
#class auto 			// Change default storage class for local variables: on the stack*****************************************************************************************************

// Redefine uC/OS-II configuration constants as necessary
#define OS_MAX_EVENTS           0       // Maximum number of events (semaphores + queues + mailboxes + MAX_TCP_SOCKET_BUFFERS + 2)
#define OS_MAX_TASKS            2  		// Maximum number of tasks system can create (less stat and idle tasks)

#define OS_MEM_EN               1   
#define OS_TASK_CREATE_EN		1       // Enable normal task creation
#define OS_TASK_CREATE_EXT_EN	0       // Disable extended task creation
#define OS_TIME_DLY_HMSM_EN	    1		// Enable OSTimeDlyHMSM
#define STACK_CNT_512	        2       // number of 512 byte stacks (application tasks + stat task + prog stack)
#define STACK_CNT_2K         	2		// TCP/IP needs a 2K stack
#define TCP_BUF_SIZE            4096	// Make the TCP tx and rx buffers 4K each
#define MAX_TCP_SOCKET_BUFFERS  0		// One sockets for TCPIP connection.

#use LAB5_SYSTEM.LIB
#use LAB5_IO.LIB
#use LAB5_MENU.LIB
#use LAB5_CLOCK.LIB
#use LAB5_EVENT.LIB
#use LAB5_PIC.LIB
#use LAB5_UCOS.LIB
#use LAB5_ETHERNET.LIB
#use LAB5_GPS_Custom.LIB
#use LAB5_GPRS

/*
*********************************************************************************************************
*                                              CONSTANTS
*********************************************************************************************************
*/

#define          TASK_STK_SIZE     512                  /* Size of each task's stacks (# of bytes)       */

#define          TASK_1_ID           1                  /* Application tasks IDs                         */
#define          TASK_2_ID           2

#define          TASK_1_PRIO        11                  /* Application tasks priorities                  */
#define          TASK_2_PRIO        12

#define MESSAGE_LENGTH              100

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void reciveTask(void *data);
void sendTask(void *data);


/*
*********************************************************************************************************
*                                                  MAIN
*********************************************************************************************************
*/

void main (void) {
    initSystem();

    OSInit();

    OSTaskCreate(reciveTask, NULL, 2048, TASK_1_PRIO);
	OSTaskCreate(sendTask, NULL, 2048, TASK_2_PRIO);

    OSStart();                                             
}

/*
*********************************************************************************************************
*                                               TASK #1
*
* Description: This task read the serial port D and prints the message recived in console.
*********************************************************************************************************
*/

void  reciveTask (void *pdata) {
    char message[MESSAGE_LENGTH];
    
    while(1) {
        readMessage(message);

        printf("%s", message);

        setOutput(PORT_E, BIT_5, 0);
    }
}

/*
*********************************************************************************************************
*                                               TASK #2
*
* Description: This task read the console input and send it by serial port D to the modem.
*********************************************************************************************************
*/

void  sendTask (void *data) {
    char message[MESSAGE_LENGTH];

    while(1) {

        while (!getswf(message)) {
            OSTimeDlyHMSM(0, 0, 0, 100);
        }

        sendMessage(message);

        OSTimeDly(5);
    }
}
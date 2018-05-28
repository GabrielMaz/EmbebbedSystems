/*******************************************************************************************************

This program does the following:

Start up task:  Creates 5 tasks.
task 1:     	Turn on red led for 800 ms and turn it off for 400 ms.
task 2:			Excecute ticks to tcp connection.
task 3:			Excecute program by console.
task 4:			Excecute program by ethernet.
Task 5.         Check for events.
					

********************************************************************************************************/
#class auto 			// Change default storage class for local variables: on the stack*****************************************************************************************************

// Redefine uC/OS-II configuration constants as necessary
#define OS_MAX_EVENTS           5       // Maximum number of events (semaphores, queues, mailboxes)
#define OS_MAX_TASKS            4  		// Maximum number of tasks system can create (less stat and idle tasks)

#define OS_MEM_EN               1   
#define OS_TASK_CREATE_EN		1       // Enable normal task creation
#define OS_TASK_CREATE_EXT_EN	0       // Disable extended task creation
#define OS_TIME_DLY_HMSM_EN	    1		// Enable OSTimeDlyHMSM
#define STACK_CNT_512	        7       // number of 512 byte stacks (application tasks + stat task + prog stack)
#define STACK_CNT_2K         	5		// TCP/IP needs a 2K stack
#define TCP_BUF_SIZE            4096	// Make the TCP tx and rx buffers 4K each
#define MAX_TCP_SOCKET_BUFFERS  1		// One sockets for TCPIP connection.

#use LAB4_IO.LIB
#use LAB4_MENU.LIB
#use LAB4_CLOCK.LIB
#use LAB4_EVENT.LIB
#use LAB4_SYSTEM.LIB
#use LAB4_PIC.LIB
#use LAB4_UCOS.LIB
#use LAB4_ETHERNET.LIB

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

#define          TASK_START_PRIO    10                /* Application tasks priorities                  */
#define          TASK_1_PRIO        11
#define          TASK_2_PRIO        12
#define          TASK_3_PRIO        13
#define          TASK_4_PRIO        14
#define          TASK_5_PRIO        15

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void     redLedTask(void *data);
void     tickTask(void *data);
void     ethernetTask(void *data);
void     checkEventsTask(void *data);


/*
*********************************************************************************************************
*                                                  MAIN
*********************************************************************************************************
*/

void main (void)
{
    initSystem();

    OSInit();                                              

    initSocket();

    OSTaskCreate(redLedTask, NULL, 2048, TASK_1_PRIO);
	OSTaskCreate(tickTask, NULL, 2048, TASK_2_PRIO);
    OSTaskCreate(ethernetTask, NULL, 2048, TASK_3_PRIO);
	OSTaskCreate(checkEventsTask, NULL, 2048, TASK_4_PRIO);

    OSStart();                                             
}

/*
*********************************************************************************************************
*                                               TASK #1
*
* Description: This task turn on red led for 800 ms and turn it off for 400 ms.
*********************************************************************************************************
*/

void  redLedTask (void *pdata)
{
    while(1) {
        OSTimeDlyHMSM(0, 0, 0, 800);
        setOutput(PORT_E, BIT_5, 1);
        OSTimeDlyHMSM(0, 0, 0, 400); 
        setOutput(PORT_E, BIT_5, 0);

        printf("task1\n");
    }
}

/*
*********************************************************************************************************
*                                               TASK #2
*
* Description: This task excecute ticks to tcp connection.
*********************************************************************************************************
*/

void  tickTask (void *data)
{
    while(1) {
        tcp_tick(&socket);

        printf("task2\n");

        OSTimeDly(5);
    }
}

/*
*********************************************************************************************************
*                                               TASK #3
*
* Description: This task excecute program by ethernet.
*********************************************************************************************************
*/

void  ethernetTask (void *data)
{
    while(1) {
        selectOption(current_state, ETHERNET);

        printf("task3\n");

        OSTimeDly(5);
    }
}

/*
*********************************************************************************************************
*                                               TASK #4
*
* Description: This task check for events.
*********************************************************************************************************
*/

void  checkEventsTask (void *data)
{
    while(1) {
        checkEventsActivated();

        printf("task4\n");

        OSTimeDly(5);
    }
}
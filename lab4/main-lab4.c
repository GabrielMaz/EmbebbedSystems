/*******************************************************************************************************

This program does the following:

Start up task: Sets up stdio window, creates 5 tasks, updates system statistics in
					the stdio window every every 5 seconds.
task 1:     	Turn on red led for 800 ms and turn it off for 400 ms.
task 2:			Excecute ticks to tcp connection.
task 3:			Excecute program by console.
task 4:			Excecute program by ethernet.
Task 5.         Check for events.
					

********************************************************************************************************/
#class auto 			// Change default storage class for local variables: on the stack*****************************************************************************************************

// Redefine uC/OS-II configuration constants as necessary
#define OS_MAX_EVENTS           2       // Maximum number of events (semaphores, queues, mailboxes)
#define OS_MAX_TASKS            6  		// Maximum number of tasks system can create (less stat and idle tasks)

#define OS_MEM_EN               1   
#define OS_TASK_CREATE_EN		0       // Disable normal task creation
#define OS_TASK_CREATE_EXT_EN	1       // Enable extended task creation
#define OS_MBOX_EN				1		// Enable mailboxes
#define OS_MBOX_POST_EN			1		// Enable MboxPost
#define OS_TIME_DLY_HMSM_EN	    1		// Enable OSTimeDlyHMSM
#define STACK_CNT_512	        8       // number of 512 byte stacks (application tasks + stat task + prog stack)
#define STACK_CNT_2K         	2		// TCP/IP needs a 2K stack
#define TCP_BUF_SIZE            4096	// Make the TCP tx and rx buffers 4K each
#define MAX_TCP_SOCKET_BUFFERS  1		// One sockets for TCPIP connection.

#use LAB4_IO.LIB
#use LAB4_MENU.LIB
#use LAB4_CLOCK.LIB
#use LAB4_EVENT.LIB
#use LAB4_SYSTEM.LIB
#use LAB4_PIC.LIB
#use LAB4_ETHERNET.LIB
#use LAB4_UCOS.LIB

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

void     TaskStartCreateTasks(void);
void     Task1(void *data);
void     Task2(void *data);
void     Task3(void *data);
void     Task4(void *data);
void     Task5(void *data);


/*
*********************************************************************************************************
*                                                  MAIN
*********************************************************************************************************
*/

void main (void)
{
    initSystem();

    OSInit();                                              

    OSTaskCreateExt(TaskStartCreateTasks,
                   (void *)0,
                   TASK_START_PRIO,
                   TASK_START_ID,
                   TASK_STK_SIZE,
                   (void *)0,
                   OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    initSocket();

    OSStart();                                             
}

/*
*********************************************************************************************************
*                                             CREATE TASKS
*********************************************************************************************************
*/

static  void  TaskStartCreateTasks (void)
{
    OSTaskCreateExt(Task1,
                   (void *)0,
                   TASK_1_PRIO,
                   TASK_1_ID,
                   512,
                   (void *)0,
                   OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskCreateExt(Task2,
                   (void *)0,
                   TASK_2_PRIO,
                   TASK_2_ID,
                   512,
                   (void *)0,
                   OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskCreateExt(Task3,
                   (void *)0,
                   TASK_3_PRIO,
                   TASK_3_ID,
                   2048,
                   (void *)0,
                   OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskCreateExt(Task4,
                   (void *)0,
                   TASK_4_PRIO,
                   TASK_4_ID,
                   2048,
                   (void *)0,
                   OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskCreateExt(Task5,
                   (void *)0,
                   TASK_5_PRIO,
                   TASK_5_ID,
                   512,
                   (void *)0,
                   OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
/*
    OSTaskCreate(Task1,NULL,2048,0);
	OSTaskCreate(Task2,NULL,2048,1);
	OSTaskCreate(Task3,NULL,2048,2);
    OSTaskCreate(Task4,NULL,2048,3);
	OSTaskCreate(Task5,NULL,2048,4);*/
}

/*
*********************************************************************************************************
*                                               TASK #1
*
* Description: This task turn on red led for 800 ms and turn it off for 400 ms.
*********************************************************************************************************
*/

void  Task1 (void *pdata)
{
    while(1) {
        OSTimeDlyHMSM(0, 0, 0, 800);
        setOutput(PORT_E, BIT_5, 1);
        OSTimeDlyHMSM(0, 0, 0, 400); 
        setOutput(PORT_E, BIT_5, 0);

        printf("trask1\n");

        OSTimeDly(5);
    }
}

/*
*********************************************************************************************************
*                                               TASK #2
*
* Description: This task excecute ticks to tcp connection.
*********************************************************************************************************
*/

void  Task2 (void *data)
{
    while(1) {
        tcp_tick(&socket);

        printf("trask2\n");

        OSTimeDly(5);
    }
}

/*
*********************************************************************************************************
*                                               TASK #3
*
* Description: This task excecute program by console.
*********************************************************************************************************
*/

void  Task3 (void *data)
{
    while(1) {
        selectOption(current_state, CONSOLE);

        printf("trask3\n");

        OSTimeDly(5);
    }
}

/*
*********************************************************************************************************
*                                               TASK #4
*
* Description: This task excecute program by ethernet
*********************************************************************************************************
*/

void  Task4 (void *data)
{
    while(1) {
        selectOption(current_state, ETHERNET);

        printf("trask4\n");

        OSTimeDly(5);
    }
}

/*
*********************************************************************************************************
*                                               TASK #5
*
* Description: This task check for events.
*********************************************************************************************************
*/

void  Task5 (void *data)
{
    while(1) {
        checkEventsActivated();

        printf("trask5\n");

        OSTimeDly(5);
    }
}
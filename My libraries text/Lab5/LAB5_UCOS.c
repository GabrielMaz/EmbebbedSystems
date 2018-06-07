/*** BeginHeader */

#define DELAY100MS() (ucosDelay(0, 0, 0, 100))
#define DELAY_MS(ms) (ucosDelay(0, 0, 0, ms))

#use "ucos2.lib"

OS_EVENT *clockSem;
UBYTE err;

/*** EndHeader */

/*** BeginHeader ucosDelay */
void ucosDelay(int hours, int minutes, int seconds, int millis);
/*** EndHeader */

void ucosDelay(int hours, int minutes, int seconds, int millis) {
    OSTimeDlyHMSM (hours, minutes, seconds, millis);
}
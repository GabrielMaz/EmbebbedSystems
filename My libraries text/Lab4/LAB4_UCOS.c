/*** BeginHeader */

#use "ucos2.lib"

OS_EVENT        *randomSem;
UBYTE err;

/*** EndHeader */

/*** BeginHeader ucosDelay */
void ucosDelay(int hours, int minutes, int seconds, int millis);
/*** EndHeader */

void ucosDelay(int hours, int minutes, int seconds, int millis) {
    OSTimeDlyHMSM (hours, minutes, seconds, millis);
}
/*** BeginHeader sendMessage*/
void sendMessage(char *message);
/*** EdnHeader */

void sendMessage(char *message) {
    serDputs(message);
    serDputc('\n');
}

/*** BeginHeader readMessage*/
void readMessage(char *result);
/*** EdnHeader */

void readMessage(char *result) {
    char result[100];
    int index;

    character = serDget();
    index = 0;

    while (1) {
        if (character = -1) {
            continue;

        } else {
            (*result)[index++] = character;
        }
    }
}
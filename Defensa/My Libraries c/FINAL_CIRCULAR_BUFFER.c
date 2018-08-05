/*** BeginHeader */

struct Information {
    int moment;
    char someData[15];
};

struct CB {
    int write;
    int read;
    int buff_size;
    struct Information information[480];
};

struct CB circular_buffer;

/*** EndHeader */

/*** BeginHeader initCircularBuffer */
void initCircularBuffer();
/*** EndHeader */

void initCircularBuffer() {
    if (!bufferCreated()) {
        circular_buffer.write = 0;
        circular_buffer.read = 0;
        circular_buffer.buff_size = 479;
    }
}

/*** BeginHeader insertCircularBuffer */
void insertCircularBuffer(struct Information *info);
/*** EndHeader */

void insertCircularBuffer(struct Information *info) {
    circular_buffer.information[circular_buffer.write] = (*info);
    
    if (circular_buffer.write == circular_buffer.buff_size) {
        circular_buffer.write = 0;

    } else {
        circular_buffer.write += 1;
    }
}

/*** BeginHeader bufferCreated */
int bufferCreated();
/*** EndHeader */

int bufferCreated() {

    // If buff_size is 479, it is already created
    if (circular_buffer.buff_size == 479) {
        return 1;
    }

    return 0;
}

/*** BeginHeader getInfo */ 
void getInfo(struct Information *info);
/*** Endheader */

void getInfo(struct Information *info) {

    info = circular_buffer.information[circular_buffer.read];
    
    if (circular_buffer.read == circular_buffer.buff_size) {
        circular_buffer.read = 0;

    } else {
        circular_buffer.read += 1;
    }
}
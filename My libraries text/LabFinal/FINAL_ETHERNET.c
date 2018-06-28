/*** BeginHeader */

#define TCPCONFIG       0

#define USE_ETHERNET	1
#define MY_IP_ADDRESS   "10.10.6.100"
#define MY_NETMASK      "255.255.255.0"
#define MY_GATEWAY      "10.10.6.2"

#define CONNECTION_DIM 50

#define CLEAR_BUFFER() (memset(&buffer_msg, '\0', 250));

#define CLEAR_SOCKET() (sock_fastread(&socket, buffer, 100));

#memmap xmem
#use "dcrtcp.lib"

#define PORT 7

tcp_Socket socket;
char buffer[100];
char buffer_msg[250];
int status;

/*** EndHeader */


/*** BeginHeader initSocket*/
void initSocket();
/*** EndHeader */

void initSocket() {

    printf("initSocket\n");
    tcp_listen(&socket,PORT,0,0,NULL,0);
    while(!sock_established(&socket) && (sock_bytesready(&socket)==-1))
    {
        tcp_tick(&socket);
        OSTimeDlyHMSM(0,0,0,100);
    }
    printf("Conectado\n");
    sock_mode(&socket,TCP_MODE_ASCII);
}

/*** BeginHeader clearScreenEthernet */
void clearScreenEthernet();
/*** EndHeader */

void clearScreenEthernet() {
    if (tcp_tick(&socket)) {
        sprintf(buffer_msg, "\n\n****************************************\n");
        sock_puts(&socket,buffer_msg);
        CLEAR_BUFFER();
    }
}

/*** BeginHeader printEthernet */
void printEthernet();
/*** EndHeader */

void printEthernet(char *message) {
    if (tcp_tick(&socket)) {
        sprintf(buffer_msg, message);
        sock_puts(&socket,buffer_msg);
        CLEAR_BUFFER();
    }
}

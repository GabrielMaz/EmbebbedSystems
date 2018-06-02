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
    printf("Iniciando Socket\n");
	sock_init();
	printf("Iniciado\n");

    tcp_listen(&socket,PORT,0,0,NULL,0);
    sock_wait_established(&socket,sock_delay,NULL,&status)
    printf("Conectado\n");
    sock_mode(&socket,TCP_MODE_ASCII);

    sock_err:
    switch(status) {
        case 1: /* foreign host closed */
            printf("User closed session\n");
            break;
        case -1: /* time-out */
            printf("Connection timed out\n");
            break;
    }
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
#define TCPCONFIG 0

#define USE_ETHERNET		1
#define MY_IP_ADDRESS "10.10.6.100"
#define MY_NETMASK "255.255.255.0"
#define MY_GATEWAY "10.10.6.2"


#memmap xmem
#use "dcrtcp.lib"

#define PORT 7

tcp_Socket echosock;

main()
{
	char buffer[2048];
	int status;

	printf("Iniciando Socket\n");
	sock_init();
	printf("Iniciado\n");

	while(1)
	{
		tcp_listen(&echosock,PORT,0,0,NULL,0);
		sock_wait_established(&echosock,sock_delay,NULL,&status)
		printf("Conectado\n");
		sock_mode(&echosock,TCP_MODE_ASCII);
		while(tcp_tick(&echosock))
		{
			/*sprintf(buffer, "respuesta");
			sock_puts(&echosock,buffer);*/
			
			sock_wait_input(&echosock,0,NULL,&status);
			if(sock_gets(&echosock,buffer,2048))
			{
				sprintf(buffer, "\n\n");
				sock_puts(&echosock,buffer);
			}
		}

		sock_err:
		switch(status)
		{
			case 1: /* foreign host closed */
				printf("User closed session\n");
				break;
			case -1: /* time-out */
				printf("Connection timed out\n");
				break;
		}
	}
}
/***************************************************************************
*
*    Last Modyfied by Charles Zhang 2017 March 15th
*    chuanyu.zhang@siemens.com
*
***************************************************************************/
#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include "Gripper.h"

#define MAXSIZE 1024

class tcp_server
{
private:
	int socket_fd,accept_fd;
	sockaddr_in myserver;
	sockaddr_in remote_addr;

public:
	tcp_server(int listen_port);
	int recv_msg(Gripper *gripper);
};

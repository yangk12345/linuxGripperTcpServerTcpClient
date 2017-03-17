/***************************************************************************
*
*    Last Modyfied by Charles Zhang 2017 March 15th  
*    chuanyu.zhang@siemens.com
* 
***************************************************************************/
#include "tcp_server.h"
#include <stdio.h>
#define Param_num 4
#define Param_long 4  // In case of corruption, Param length should not be longer than MAX_INT>>8 
#define Data_Freq 10
#define Action_Freq 2  // Data_Freq/Action Freq must be an intiger


tcp_server::tcp_server(int listen_port){

	if((socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) <0 ){
		throw "socket() failed";
	}

	memset(&myserver, 0, sizeof(myserver));
	myserver.sin_family = AF_INET;
	myserver.sin_addr.s_addr = htonl(INADDR_ANY);
	myserver.sin_port = htons(listen_port);

	//printf("point 1\n");
	if(bind(socket_fd, (sockaddr*)&myserver, sizeof(myserver)) <0) {
		printf("bind failed!\n");
		throw "bind() failed";
	}

	if(listen(socket_fd, 10) < 0){
		throw "listen failed";
	}
}

int tcp_server::recv_msg(Gripper *gripper) {

	int i=0,j=0,num=0,count=0;;
 	char Param[Param_num][Param_long]={0};
	int stop_flag=0;
	int RPosition=0,RSpeed=0,RForce=0;

	int Max_input=0,Min_input=9999;

	while(1) {
		//printf("point 2\n");
		socklen_t sin_size = sizeof(struct sockaddr_in);
		if(( accept_fd = accept(socket_fd, (struct sockaddr*)&remote_addr, &sin_size)) == -1)
		{
			printf("Accept error!\n");
			continue;
		}

		printf("Recieved a connection from %s\n", (char*) inet_ntoa(remote_addr.sin_addr));

		char buffer[MAXSIZE];
		while(1) {
			memset(buffer,0,MAXSIZE);
			//printf("point 3\n");
			if(( read(accept_fd, buffer, MAXSIZE))<0 ){
				throw("Read() error!");
			} else {
				if(memcmp(buffer,"exit",4)==0){ 
					close(accept_fd);
					goto end;
					break;
				}
				else
				{
					//printf("Recieved:%s\n", buffer);

					/*seperate buffer data into Param_num parts*/
					while((buffer[num]!='\0')&&(i<Param_num)){
						if((buffer[num]!=' ')&&(j<Param_long)){
							Param[i][j]=buffer[num];
							j++;
							stop_flag=0;
						}
						else if((buffer[num]==' ')&&(stop_flag==0)){
							i++;
							j=0;
							stop_flag=1;
						}
						num++;
					}/*seperater end*/

					/*Filter*/
					RPosition+=atoi(Param[0]);
					RSpeed+=atoi(Param[1]);
					RForce+=atoi(Param[2]);

					if(count>(Data_Freq/Action_Freq-1)){

						RPosition/=(Data_Freq/Action_Freq);
						RSpeed/=(Data_Freq/Action_Freq);
						RForce/=(Data_Freq/Action_Freq);
						
						if(RPosition>1800)
							RPosition=1800;
						if(RPosition<1200)
							RPosition=1200;

						if(Max_input<RPosition){
							Max_input=RPosition;
							printf("Max=%d,Min=%d\n",Max_input,Min_input);
						}
						if(Min_input>RPosition){
							Min_input=RPosition;
							printf("Max=%d,Min=%d\n",Max_input,Min_input);
						}

						if(Max_input!=Min_input)
							RPosition=((RPosition-Min_input)*255)/(Max_input-Min_input);	
						printf("Required Position=%d\nRequired Speed=%d\nRequired Force=%d\n",RPosition,RSpeed,RForce);

						gripper->go(false);
						gripper->setPosition(255-RPosition);
						if(RSpeed)
							gripper->setSpeed(RSpeed);
						if(RForce)
							gripper->setForce(RForce);
						gripper->go(true);

						printf("actual position=%d\n",gripper->getPosition());
						/*action mode end*/

						RPosition=0;	
						RSpeed=0;	
						RForce=0;	
						count=0;
					}
					count++;
					/*Filter end*/

					/*clear register*/
					for(i=0;i<3;i++){
						for(j=0;j<3;j++){
							Param[i][j]=0;
						}
					}
					i=0;
					j=0;
					num=0;
					stop_flag=0;
					
					continue;
				}
			}
		}
	}
end:
	close(accept_fd);
	return 0;
}

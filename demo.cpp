/***************************************************************************
*
*    Last Modyfied by Charles Zhang 2017 March 15th
*    chuanyu.zhang@siemens.com
*
***************************************************************************/
#include "Gripper.h"
#include <pthread.h>
#include "tcp_server.h"
#define sec 1000000 //!<sec in microseconds
#define ms  1000    //!<ms in microseconds
#define PORT 1234

//!thread purposes
enum Thread {Com, Fault};
pthread_t launch(Gripper*, Thread);  //!<thread launcher
void terminate(pthread_t);	 //!<thread terminator

int main(int argc, char*argv[] ) {

	Gripper gripper;
	pthread_t com=launch(&gripper,Com); //!<start communication
	pthread_t fault=launch(&gripper,Fault); //!<start fault monitoring

	//!Connect
	while(!gripper.isConnected())
	{
		gripper.connect("/dev/ttyUSB0", 115200, 'N', 8, 1);
		usleep(sec);

	}cout << "connected" <<endl;

        tcp_server ts(PORT);

	//!Activation
	//cout << "Activating in Basic mode" <<endl;
	while(!gripper.isActivated()){

		gripper.activate();
		usleep(sec);

	}cout << "activated" <<endl<<endl;

	if(argv[1]==NULL)
		gripper.setSpeed(100);
	else
		gripper.setSpeed(atoi(argv[1]));
	if(argv[2]==NULL)
		gripper.setForce(100);
	else
		gripper.setForce(atoi(argv[2]));

        ts.recv_msg(&gripper);
	
	//gripper.activate(Gripper::Wide);
	//
	//			while(gripper.getMode()!=Gripper::Wide){
	//
	//						usleep(ms);
	//	
	//						}cout<<"changing mode"<<endl;
	//
	//		while(!gripper.isActivated()){
	//
	//						usleep(ms);
	//
	//						}cout<<"mode changed"<<endl<<endl;
	//


	//cout << "Perform individual control of fingers, proceed?" ;
	//cin.get();

	//gripper.go(false);
	//gripper.setIndividualCtrl(true);
	//gripper.a.setPosition(70);
	//gripper.go(true);


	//		while(!gripper.isMoving()){
	//
	//						 usleep(ms);
	//
	//						 }cout<<"Opening finger A"<<endl;
	//
	//		while(gripper.isMoving()){
	//
	//						 usleep(ms);
	//
	//						 }cout<<"position reached"<<endl<<endl;
	//


	gripper.go(false);
	gripper.clear();
	gripper.go(true);

	gripper.deactivate();
	cout<<"deactivated"<<endl;
	gripper.disconnect();
	cout<<"disconnected"<<endl;

	terminate(fault);
	terminate(com);
	return 0;
}



void* com(void* gripper){

	while(true)
	{
		((Gripper*)gripper)->synchronise(Gripper::Dual);
		usleep(ms);
	}

}

void* fault(void* gripper){

	while(true)
	{
		if(((Gripper*)gripper)->getFaultStatus()!=Gripper::NoFault)
		cout << ((Gripper*)gripper)->getFaultMsg() <<endl;

		usleep(sec);
	}

}



pthread_t launch(Gripper* gripper, Thread type ){

	pthread_t tid;

	if(type==Com)
	pthread_create(&tid, NULL,com,(void*)gripper);
	else pthread_create(&tid, NULL,fault,(void*)gripper);

	return tid;
}

void terminate(pthread_t id){

	pthread_cancel(id);
	pthread_join(id,NULL);


}

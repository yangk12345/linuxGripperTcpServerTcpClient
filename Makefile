all:
	g++ demo.cpp src/Gripper.cpp src/tcp_server/tcp_server.cpp -I src/ -I src/tcp_server/ -L src/modbus/ -lpthread -lmodbus

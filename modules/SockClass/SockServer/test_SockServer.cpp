//server.cpp
#include <iostream>
#include "SockServer.h"

int main() {

	SockServer sockserver("127.0.0.1", 1234);
	sockserver.Connect();
	std::string str = sockserver.Recv();
	std::cout << "Message from client:\n" << str << std::endl;
	sockserver.Send(str);
	sockserver.Disconnect();

	return 0;
}
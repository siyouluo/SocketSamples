//client.cpp
#include <iostream>
#include "SockClient.h"

int main() {
	std::string cmdstr("hello world");
	std::cout << "Cmd:\n" << cmdstr << std::endl;

	SockClient sockclient("127.0.0.1", 1234);
	sockclient.Connect();
	sockclient.Send(cmdstr);
	std::string str = sockclient.Recv();
	std::cout << "Message from server:\n" << str << std::endl;
	sockclient.Disconnect();
	
	system("pause");
	return 0;
}
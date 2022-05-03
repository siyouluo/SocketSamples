#pragma once
#include <string>
#include <WinSock2.h>
#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")  //加载 ws2_32.dll

class SockClient
{
public:
	SockClient();
	SockClient(std::string server_ip, unsigned short server_port);
	~SockClient();
	void SockClient_Init();
	void SockClient_Init(std::string server_ip, unsigned short server_port);
	bool Connect();
	void Disconnect();
	void Send(const char *buf, int len);
	void Send(std::string str);
	void Recv(char *buf, int len);
	std::string Recv();
private:
	std::string m_server_ip;
	unsigned short m_server_port;
	//创建套接字
	SOCKET m_sock;
	//向服务器发起请求
	sockaddr_in m_sockAddr;
	bool GraceClose();
};


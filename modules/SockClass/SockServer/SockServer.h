//SockServer.h
#pragma once
#include <string>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")  //加载 ws2_32.dll

class SockServer
{
public:
	SockServer();
	SockServer(std::string server_ip, unsigned short server_port);
	~SockServer();
	void SockServer_Init();
	void SockServer_Init(std::string server_ip, unsigned short server_port);
	void Connect();
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
	SOCKET m_clientsock;
	//服务器套接字地址
	sockaddr_in m_sockAddr;
};

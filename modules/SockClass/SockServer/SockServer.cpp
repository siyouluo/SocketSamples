//SockServer.cpp
/*************************************************************************************
 * @Info: 对WinSock2进行封装,默认采用TCP连接,可以在类构造函数中就把服务器的ip:port初始化:
 *			SockServer sockserver("127.0.0.1", 1234);
 *		  也可以先构造,然后再初始化:
 *			SockServer sockserver;
 *			SockServer_Init("127.0.0.1", 1234);
 *		  与客户端通信前要先监听客户端并接受请求:
 *			Connect();
 *		  发送数据:
 *			1. Send("hello world",12);
 *			2. Send(std::string("hello world"));
 *		  接收数据:
 *			1. char szBuffer[MAX_RECVBUF_SIZE] = { 0 };
 *			   Recv(szBuffer, MAX_RECVBUF_SIZE);
 *			2. std::string str = Recv();
 *		  断开连接,这里把客户端和服务器端套接字都关闭了,如果服务器还要等待其他连接请求,则可以不用关闭服务器端
 *			Disconnect();
 *************************************************************************************/
#include "SockServer.h"

#define MAX_RECVBUF_SIZE 1024
SockServer::SockServer()
{
	//初始化DLL
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	m_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&m_sockAddr, 0, sizeof(m_sockAddr));  //每个字节都用0填充
}

SockServer::SockServer(std::string server_ip, unsigned short server_port)
	:m_server_ip(server_ip), m_server_port(server_port)
{
	//初始化DLL
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	m_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&m_sockAddr, 0, sizeof(m_sockAddr));  //每个字节都用0填充
	SockServer_Init();
}

SockServer::~SockServer()
{
	//终止使用 DLL
	WSACleanup();
}

void SockServer::SockServer_Init()
{
	SockServer_Init(m_server_ip, m_server_port);
}

void SockServer::SockServer_Init(std::string server_ip, unsigned short server_port)
{
	m_server_ip = server_ip;
	m_server_port = server_port;
	m_sockAddr.sin_family = PF_INET;
	m_sockAddr.sin_addr.s_addr = inet_addr(server_ip.c_str());
	m_sockAddr.sin_port = htons(server_port);
	bind(m_sock, (SOCKADDR*)&m_sockAddr, sizeof(SOCKADDR));
}

void SockServer::Connect()
{
	//进入监听状态
	listen(m_sock, 20);

	//接收客户端请求
	SOCKADDR clntAddr;
	int nSize = sizeof(SOCKADDR);
	m_clientsock = accept(m_sock, (SOCKADDR*)&clntAddr, &nSize);
}

void SockServer::Send(std::string str)
{
	send(m_clientsock, str.c_str(), str.size(), NULL);//向服务器发送命令
}

void SockServer::Send(const char *buf, int len)
{
	send(m_clientsock, buf, len, NULL);
}


void SockServer::Disconnect()
{
	//关闭套接字
	closesocket(m_clientsock);
	closesocket(m_sock);
}

void SockServer::Recv(char *buf, int len)
{
	recv(m_clientsock, buf, len, NULL);
}

std::string SockServer::Recv()
{
	char szBuffer[MAX_RECVBUF_SIZE] = { 0 };
	recv(m_clientsock, szBuffer, MAX_RECVBUF_SIZE, NULL);
	return std::string(szBuffer);
}

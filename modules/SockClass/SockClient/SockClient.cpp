//SockClient.cpp
/*************************************************************************************
 * @Info: 对WinSock2进行封装,默认采用TCP连接,可以在类构造函数中就把服务器的ip:port初始化:
 *			SockClient sockclient("127.0.0.1", 1234);
 *		  也可以先构造,然后再初始化:
 *			SockClient sockclient;
 *			SockClient_Init("127.0.0.1", 1234);
 *		  往服务器发送数据前要先连接:
 *			Connect();
 *		  发送数据:
 *			1. Send("hello world",12);
 *			2. Send(std::string("hello world"));
 *		  接收数据:
 *			1. char szBuffer[MAX_RECVBUF_SIZE] = { 0 };
 *			   Recv(szBuffer, MAX_RECVBUF_SIZE);
 *			2. std::string str = Recv();
 *		  断开连接:
 *			Disconnect();
 *************************************************************************************/
#include "SockClient.h"
#define MAX_RECVBUF_SIZE 1024

SockClient::SockClient()
{
	//初始化DLL
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	m_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&m_sockAddr, 0, sizeof(m_sockAddr));  //每个字节都用0填充
}

SockClient::SockClient(std::string server_ip, unsigned short server_port)
	:m_server_ip(server_ip),m_server_port(server_port)
{
	//初始化DLL
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	m_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&m_sockAddr, 0, sizeof(m_sockAddr));  //每个字节都用0填充

	SockClient_Init();
}

SockClient::~SockClient()
{
	//终止使用 DLL
	//在Windows下，Socket是以DLL的形式实现的.
	//在DLL内部维持着一个计数器，只有第一次调用WSAStartup才真正装载DLL，以后的调用只是简单的增加计数器.
	//而WSACleanup函数的功能则刚好相反，每调用一次使计数器减1，当计数器减到0时，DLL就从内存中被卸载！
	//因此，调用了多少次WSAStartup，就应相应的调用多少次的WSACleanup.
	WSACleanup();
}
void SockClient::SockClient_Init()
{
	SockClient_Init(m_server_ip, m_server_port);
}

void SockClient::SockClient_Init(std::string server_ip, unsigned short server_port)
{
	m_server_ip = server_ip;
	m_server_port = server_port;
	m_sockAddr.sin_family = PF_INET;
	m_sockAddr.sin_addr.s_addr = inet_addr(server_ip.c_str());
	m_sockAddr.sin_port = htons(server_port);
}

void SockClient::Connect()
{
	connect(m_sock, (SOCKADDR*)&m_sockAddr, sizeof(SOCKADDR));
}
void SockClient::Disconnect()
{
	//关闭套接字
	closesocket(m_sock);
}

void SockClient::Send(std::string str)
{
	send(m_sock, str.c_str(), str.size(), NULL);//向服务器发送命令
}

void SockClient::Send(const char *buf,int len)
{
	send(m_sock, buf, len, NULL);//向服务器发送命令
}

void SockClient::Recv(char *buf, int len)
{
	recv(m_sock, buf, len, NULL);
}

std::string SockClient::Recv()
{
	char szBuffer[MAX_RECVBUF_SIZE] = { 0 };
	// recv函数仅仅是将数据从套接字的缓冲区拷贝到用户提供的缓冲区szBuffer, 真正的接收数据是协议来完成的
	// 协议接收到的数据可能大于buf的长度，所以在这种情况下要调用几次recv函数才能把s的接收缓冲中的数据copy完
	recv(m_sock, szBuffer, MAX_RECVBUF_SIZE, NULL);
	return std::string(szBuffer);
}
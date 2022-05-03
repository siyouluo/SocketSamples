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
 * @Refer: 参考资料
 *		[Winsocket 三：非阻塞server&client程序（tcp）](https://blog.csdn.net/u013071074/article/details/26354781)
 *		[Winsocket入门教程二：非阻塞式服务器和客户端程序(TCP)](https://blog.csdn.net/shining100/article/details/5651878)
 *		[非阻塞式服务器和客户端程序(TCP)【简单的原理例子】](https://www.cnblogs.com/linmzh/archive/2013/01/15/2860704.html)
 *		[关于PF_INET和AF_INET的区别](https://www.cnblogs.com/wkun/p/4240472.html)
 *************************************************************************************/
#include "SockClient.h"
#include <cassert>  
#include <iostream>
#define MAX_RECVBUF_SIZE 1024

SockClient::SockClient()
{
	// 初始化Winsocket   
	int iRet = SOCKET_ERROR;
	WSADATA data;
	ZeroMemory(&data, sizeof(WSADATA));
	iRet = WSAStartup(MAKEWORD(2, 2), &data);
	assert(SOCKET_ERROR != iRet);

	// 建立连接套接字   
	m_sock = INVALID_SOCKET;
	m_sock = socket(PF_INET, SOCK_STREAM, 0);
	assert(INVALID_SOCKET != m_sock);
}

SockClient::SockClient(std::string server_ip, unsigned short server_port)
	:m_server_ip(server_ip), m_server_port(server_port)
{
	// 初始化Winsocket   
	int iRet = SOCKET_ERROR;
	WSADATA data;
	ZeroMemory(&data, sizeof(WSADATA));
	iRet = WSAStartup(MAKEWORD(2, 2), &data);
	assert(SOCKET_ERROR != iRet);

	// 建立连接套接字   
	m_sock = INVALID_SOCKET;
	m_sock = socket(PF_INET, SOCK_STREAM, 0);
	assert(INVALID_SOCKET != m_sock);
	SockClient_Init();
}

SockClient::~SockClient()
{
	//终止使用 DLL
	//在Windows下，Socket是以DLL的形式实现的.
	//在DLL内部维持着一个计数器，只有第一次调用WSAStartup才真正装载DLL，以后的调用只是简单的增加计数器.
	//而WSACleanup函数的功能则刚好相反，每调用一次使计数器减1，当计数器减到0时，DLL就从内存中被卸载！
	//因此，调用了多少次WSAStartup，就应相应的调用多少次的WSACleanup.
	//清理Winsocket资源   
	int flag = WSACleanup();
	assert(SOCKET_ERROR != flag);
}
void SockClient::SockClient_Init()
{
	SockClient_Init(m_server_ip, m_server_port);
}

void SockClient::SockClient_Init(std::string server_ip, unsigned short server_port)
{
	memset(&m_sockAddr, 0, sizeof(m_sockAddr));  //每个字节都用0填充
	m_server_ip = server_ip;
	m_server_port = server_port;
	m_sockAddr.sin_family = AF_INET;
	m_sockAddr.sin_addr.s_addr = inet_addr(server_ip.c_str());
	m_sockAddr.sin_port = htons(server_port);
}

bool SockClient::Connect()
{
	int flag;
	// 将套接口从阻塞状态设置到非阻塞状态   
	unsigned long ulEnable = 1;//1表示非阻塞，0表示阻塞
	flag = ioctlsocket(m_sock, FIONBIO, &ulEnable);
	assert(SOCKET_ERROR != flag);

	FD_SET fsWrite;
	TIMEVAL tv;
	tv.tv_sec = 1;//秒
	tv.tv_usec = 0;//微秒

	bool success = false;
	std::cout << "Client begin to connect to the server..." << std::endl;
	for (size_t i = 0; i < 3; i++)
	{
		// 使用非阻塞方式连接服务器，请注意connect操作的返回值总是为SOCKET_ERROR  
		// 当然也有可能是0(当尝试连接本机服务器时)
		flag = connect(m_sock, (sockaddr*)&m_sockAddr, sizeof(sockaddr_in));
		int iErrorNo = SOCKET_ERROR;
		int iLen = sizeof(int);
		// 如果getsockopt返回值不为0，则说明有错误出现   
		if (SOCKET_ERROR == flag && 0 != getsockopt(m_sock, SOL_SOCKET, SO_ERROR, (char*)&iErrorNo, &iLen))
		{
			std::cout << "An error happened on connecting to server. The error no is " << iErrorNo
				<< ". The program will exit now." << std::endl;
			exit(-1);
		}

		FD_ZERO(&fsWrite);
		FD_SET(m_sock, &fsWrite);
		// 如果集合fsWrite中的套接字有信号，则说明连接成功，此时flag的返回值大于0   
		flag = select(1, NULL, &fsWrite, NULL, &tv);
		if (flag > 0)
		{
			std::cout << "Connected to server successfully!" << std::endl;
			success = true;
			break;
		}
		std::cout << "retrying..." << std::endl;
	}  //for
	// 将套接字设置回阻塞状态,方便后续以阻塞方式进行数据传输
	ulEnable = 0;
	flag = ioctlsocket(m_sock, FIONBIO, &ulEnable);
	assert(SOCKET_ERROR != flag);

	if (success)
	{
		std::cout << "Connected to the server successfully!" << std::endl;
	}
	else
	{
		std::cout << "Failed to connect to the server!" << std::endl;
	}
	return success;
}
void SockClient::Disconnect()
{
	// 将套接字设置回阻塞状态   
	unsigned long ulEnable = 0;
	int flag = ioctlsocket(m_sock, FIONBIO, &ulEnable);
	assert(SOCKET_ERROR != flag);

	// 关闭监听套接字   
	flag = GraceClose();
	//assert(flag);//这里就不用再assert了，随他去吧
}

void SockClient::Send(std::string str)
{
	send(m_sock, str.c_str(), (int)str.size(), NULL);//向服务器发送命令
}

void SockClient::Send(const char *buf, int len)
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

//体面退场
bool SockClient::GraceClose()
{
	const int c_iBufLen = 512;
	char szBuf[c_iBufLen + 1] = { '\0' };
	// 关闭该套接字的连接   
	int flag = shutdown(m_sock, SD_SEND);
	while (recv(m_sock, szBuf, c_iBufLen, 0) > 0);
	if (SOCKET_ERROR == flag)
	{
		return false;
	}
	// 清理该套接字的资源   
	flag = closesocket(m_sock);
	if (SOCKET_ERROR == flag)
	{
		return false;
	}
	m_sock = INVALID_SOCKET;
	return true;
}

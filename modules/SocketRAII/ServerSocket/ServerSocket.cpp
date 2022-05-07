#include "ServerSocket.h"
#include <iostream>


ServerSocket::ServerSocket()
{
    m_bInit = false;
    m_listenfd = INVALID_SOCKET;
}

ServerSocket::~ServerSocket()
{
    if ((m_listenfd != INVALID_SOCKET) && (::closesocket(m_listenfd) == SOCKET_ERROR))
    {
        std::cout << "closesocket function failed with error: " 
                << WSAGetLastError() << std::endl;
    }
    if (m_bInit && (::WSACleanup() == SOCKET_ERROR))
    {
        std::cout << "WSACleanup function failed with error: " 
                << WSAGetLastError() << std::endl;
    }
}

bool ServerSocket::DoInit()
{
	WSADATA wsaData;
	int wsa_err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsa_err != 0) 
    {
		std::cout << "WSAStartup failed with error: " << wsa_err << std::endl;
		return false;
	}
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        return false;
    }
    m_bInit = true;

	//创建监听套接字
	m_listenfd = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_listenfd == INVALID_SOCKET)
	{
		std::cout << "socket function failed with error: " 
                << WSAGetLastError() << std::endl;
		return false;
	}
    return true;
}

//绑定套接字
bool ServerSocket::DoBind(const char* ip, unsigned short port)
{
	sockaddr_in bindaddr;
	memset(&bindaddr, 0, sizeof(bindaddr));  //每个字节都用0填充
	bindaddr.sin_family = AF_INET;  //使用IPv4地址
	bindaddr.sin_addr.s_addr = inet_addr(ip);  //具体的IP地址
	bindaddr.sin_port = htons(port);  //端口
	if (::bind(m_listenfd, (SOCKADDR*)&bindaddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		std::cout << "bind function failed with error: " 
                << WSAGetLastError() << std::endl;
        return false;
	}
    return true;
}

//进入监听状态
bool ServerSocket::DoListen(int backlog)
{
	if (::listen(m_listenfd, backlog) == SOCKET_ERROR)
	{
		std::cout << "listen function failed with error: " 
                << WSAGetLastError() << std::endl;
		return false;
	}
    return true;
}

//接收客户端连接
bool ServerSocket::DoAccept()
{
    std::cout << "listening on socket ..." << std::endl;
    SOCKADDR clientaddr;
    int clientaddrlen = sizeof(clientaddr);
    m_clientfd = ::accept(m_listenfd, (SOCKADDR*)&clientaddr, &clientaddrlen);
    if (m_clientfd == INVALID_SOCKET) 
    {
        std::cout << "accept function failed with error: " 
                << WSAGetLastError() << std::endl;
        return false;
    }
    std::cout << "client connected." << std::endl;
    return true;
}

//关闭客户端连接
// false: 关闭连接失败，程序需要退出
// true: 成功关闭连接
bool ServerSocket::DoShutdown()
{
    // shutdown the connection
    if (shutdown(m_clientfd, SD_SEND) == SOCKET_ERROR) 
    {
        std::cout << "shutdown function failed with error: " 
            << WSAGetLastError() << std::endl;
        if (::closesocket(m_clientfd) == SOCKET_ERROR)
        {
            std::cout << "closesocket function failed with error: " 
                    << WSAGetLastError() << std::endl;
            return false;
        }
    }
    return true;
}
bool ServerSocket::Send(const std::string& str)
{
    return this->Send(str.c_str(), str.size()+1); // 保证'\0'也会被发送过去
}
bool ServerSocket::Send(const char* buf, int len)
{
    int sent_bytes = 0;
    int ret = 0;
    while(len > sent_bytes)
    {
        ret = ::send(m_clientfd, buf + sent_bytes, len - sent_bytes, NULL);
        if(ret == SOCKET_ERROR)
        {
            std::cout << "send function failed with error: " 
                << WSAGetLastError() << std::endl;
            return false;
        }
        else if(ret == 0)
        {
            // 如果发送的字节数 len - sent_bytes == 0, 那么返回的ret也是0，这时是正常的;
            // 如果发送的字节数不为0，而返回值ret==0, 表示对端关闭了连接，这边也关闭连接即可.
            return false;
        }
        sent_bytes += ret;
    }
    std::cout << "send " << sent_bytes << " bytes to client." << std::endl;
    return true;
}

bool ServerSocket::Recv(std::string& str)
{
    int n_to_recv = m_recv_maxsize;// 准备接收n_to_recv个字节的数据
    bool ret = this->Recv(m_recv_buff, n_to_recv);// n_to_recv会通过实参引用的方式被修改为实际接收到的字节数
    m_recv_buff[n_to_recv] = '\0'; // 如果接收到的数据里面没有'\0', 就在最后补一个'\0'
    str = std::string(m_recv_buff);
    return ret;
}

bool ServerSocket::Recv(char *buf, int& len)
{
    int n_recv = recv(m_clientfd, buf, len, NULL);
    if (n_recv > 0) 
    {
        std::cout << "receive " << n_recv << " bytes from client." << std::endl;
        return true;
    }
    else if (n_recv == 0)
    {
        // 只有在对方关闭连接时，recv才会返回0
        std::cout << "Connection closing ... \n" << std::endl;
        return false;
    }
    else 
    {
        std::cout << "recv function failed with error: " << WSAGetLastError() << std::endl;
        return false;
    }
    return true;
}

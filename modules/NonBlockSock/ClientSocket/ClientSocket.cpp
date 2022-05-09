#include "ClientSocket.h"
#include <iostream>
#include <math.h>

ClientSocket::ClientSocket()
{
    m_bInit = false;
	m_bConnected = false;
    m_clientfd = INVALID_SOCKET;
}

ClientSocket::~ClientSocket()
{
    if ((m_clientfd != INVALID_SOCKET) && (::closesocket(m_clientfd) == SOCKET_ERROR))
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

bool ClientSocket::DoInit()
{
    if(m_bInit==false)
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
    }
    if(m_clientfd == INVALID_SOCKET)
    {
        //创建套接字
        std::cout << "create client socket" << std::endl;
        m_clientfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (m_clientfd == INVALID_SOCKET) 
        {
            std::cout << "socket function failed with error: " << WSAGetLastError() << std::endl;
            return false;
        }
        m_bConnected = false;
    }
    return true;
}


bool ClientSocket::DoConnect(const char* ip, unsigned short port, int n_attempts, float timeout_sec)
{
    // convert timeout_sec type: float -> TIMEVAL
    float tv_sec, tv_usec;
    tv_usec = modf(timeout_sec, &tv_sec) * 1000000;
    TIMEVAL timeout = {(long)tv_sec, (long)tv_usec}; 

	// 将套接口从阻塞状态设置到非阻塞状态   
	unsigned long ulEnable = 1;//1表示非阻塞，0表示阻塞
    if(ioctlsocket(m_clientfd, FIONBIO, &ulEnable) == SOCKET_ERROR)
    {
        std::cout << "ioctlsocket function failed with error: " << WSAGetLastError() << std::endl;
        return false;
    }
	
	std::cout << "connect to the server ..." << std::endl;
	bool success = false;
	for (size_t i = 0; i < n_attempts; i++)
	{
		// 使用非阻塞方式连接服务器，请注意connect操作的返回值总是为SOCKET_ERROR  
		// 当然也有可能是0(当尝试连接本机服务器时)
        sockaddr_in serveraddr;
        memset(&serveraddr, 0, sizeof(serveraddr));  //每个字节都用0填充
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_addr.s_addr = inet_addr(ip);
        serveraddr.sin_port = htons(port);
        if(connect(m_clientfd, (SOCKADDR*)&serveraddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
        {
            int optVal;
            int optLen = sizeof(int);
            // 如果getsockopt返回值不为0，则说明有错误出现   
            if(getsockopt(m_clientfd, SOL_SOCKET, SO_ERROR, (char*)&optVal, &optLen) == SOCKET_ERROR)
            {
                std::cout << "getsockopt function failed with error: " << WSAGetLastError() << std::endl;
			    std::cout << "SockOpt Value: " << optVal << std::endl;
                return false;
            }
        }
        FD_SET writefds;
		FD_ZERO(&writefds);
		FD_SET(m_clientfd, &writefds);
		// 如果集合writefds中的套接字有信号，则说明连接成功，此时flag的返回值大于0   
		int ret = select(1, NULL, &writefds, NULL, &timeout);
        if(ret == 0)
        {
            std::cout << "Connect to server timeout (" << timeout_sec << " s)." << std::endl;
        }
        else if(ret == SOCKET_ERROR)
        {
            std::cout << "select function failed with error: " << WSAGetLastError() << std::endl;
        }
        else if (ret > 0)
		{
			std::cout << "Connected to server successfully!" << std::endl;
			success = true;
			break;
		}
	}  //for
	// 将套接字设置回阻塞状态,方便后续以阻塞方式进行数据传输
	ulEnable = 0;
    if(ioctlsocket(m_clientfd, FIONBIO, &ulEnable) == SOCKET_ERROR)
    {
        std::cout << "ioctlsocket function failed with error: " << WSAGetLastError() << std::endl;
        return false;
    }

	if (!success)
	{
		std::cout << "Failed to connect to the server!" << std::endl;
	}
	return success;
}

//客户端也是可以绑定端口号的
bool ClientSocket::DoBind(const char* ip, unsigned short port)
{
	sockaddr_in bindaddr;
	memset(&bindaddr, 0, sizeof(bindaddr));  //每个字节都用0填充
	bindaddr.sin_family = AF_INET;  //使用IPv4地址
	bindaddr.sin_addr.s_addr = inet_addr(ip);  //具体的IP地址
	bindaddr.sin_port = htons(port);  //端口
	if (::bind(m_clientfd, (SOCKADDR*)&bindaddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		std::cout << "bind function failed with error: " 
                << WSAGetLastError() << std::endl;
        return false;
	}
    return true;
}

//关闭客户端连接
// false: 关闭连接失败，程序需要退出
// true: 成功关闭连接
bool ClientSocket::DoShutdown()
{
    // shutdown the connection
    std::cout << "shutdown connection ..." << std::endl;
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
        m_bConnected = false;
        return false;
    }
    
    //保证将内核缓冲区内的数据全部读出
    std::string temp_str;
    while (this->Recv(temp_str) == true);

    std::cout << "close socket ... \n" << std::endl;
    if (::closesocket(m_clientfd) == SOCKET_ERROR)
    {
        std::cout << "closesocket function failed with error: " 
                << WSAGetLastError() << std::endl;
        return false;
    }
    m_clientfd = INVALID_SOCKET;
    m_bConnected = false;
    return true;
}
bool ClientSocket::Send(const std::string& str)
{
    return this->Send(str.c_str(), str.size()+1); // 保证'\0'也会被发送过去
}
bool ClientSocket::Send(const char* buf, int len)
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
    std::cout << "send " << sent_bytes << " bytes to server." << std::endl;
    return true;
}

bool ClientSocket::Recv(std::string& str)
{
    int n_to_recv = m_recv_buff_size;// 准备接收n_to_recv个字节的数据
    bool ret = this->Recv(m_recv_buff, n_to_recv);// n_to_recv会通过实参引用的方式被修改为实际接收到的字节数
    m_recv_buff[n_to_recv] = '\0'; // 如果接收到的数据里面没有'\0', 就在最后补一个'\0'
    str = std::string(m_recv_buff);
    return ret;
}

bool ClientSocket::Recv(char *buf, int& len)
{
    int n_recv = recv(m_clientfd, buf, len, NULL);
    if (n_recv > 0) 
    {
        std::cout << "receive " << n_recv << " bytes from server." << std::endl;
        return true;
    }
    else if (n_recv == 0)
    {
        // 只有在对方关闭连接时，recv才会返回0
        std::cout << "recv 0 byte: the peer closed the connection." << std::endl;
        return false;
    }
    else 
    {
        std::cout << "recv function failed with error: " << WSAGetLastError() << std::endl;
        return false;
    }
    return true;
}

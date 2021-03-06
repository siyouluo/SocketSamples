#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include <WinSock2.h>
#include <string>

class ServerSocket
{
public:
    ServerSocket();
    ~ServerSocket();

    bool DoInit();
    bool DoBind(const char* ip, unsigned short port);
    bool DoListen(int backlog = SOMAXCONN);
    bool DoAccept();
    bool DoShutdown();
    bool Send(const char* buf, int len);
    bool Send(const std::string& str);
    bool Recv(char *buf, int& len);
    bool Recv(std::string& str);
private:
    bool   m_bInit;
    SOCKET m_listenfd;
    SOCKET m_clientfd;
    static const unsigned int m_recv_buff_size = 1024;// 最多接收 m_recv_buff_size 个字符
	char m_recv_buff[m_recv_buff_size + 1] = {0};// 最后预留一个给'\0'
};

#endif /* SERVERSOCKET_H */




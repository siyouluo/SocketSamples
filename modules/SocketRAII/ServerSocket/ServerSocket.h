#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include <WinSock2.h>
#include <string>

#define RECV_BUFF_SIZE 1024

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
    char m_recv_buff[RECV_BUFF_SIZE];
    int m_recv_maxsize = RECV_BUFF_SIZE - 1 ; // 最多接收 m_recv_maxsize 个字符，最后预留一个给'\0'
};

#endif /* SERVERSOCKET_H */




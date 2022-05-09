#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <WinSock2.h>
#include <string>

class ClientSocket
{
public:
    ClientSocket();
    ~ClientSocket();

    bool DoInit();
    bool DoBind(const char* ip, unsigned short port);
    bool DoConnect(const char* ip, unsigned short port, int n_attempts=1, float timeout_sec=0.5);
    bool DoShutdown();
    bool Send(const char* buf, int len);
    bool Send(const std::string& str);
    bool Recv(char *buf, int& len);
    bool Recv(std::string& str);
private:
    bool   m_bInit;
    bool   m_bConnected;
    SOCKET m_clientfd;
    static const unsigned int m_recv_buff_size = 1024;// 最多接收 m_recv_buff_size 个字符
	char m_recv_buff[m_recv_buff_size + 1] = {0};// 最后预留一个给'\0'
};

#endif /* CLIENTSOCKET_H */




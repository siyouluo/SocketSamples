#include "ServerSocket.h"


int main(int argc, char* argv[])
{
    ServerSocket serverSocket;
    if (!serverSocket.DoInit())
        return -1;

    if (!serverSocket.DoBind("127.0.0.1", 1234))
        return -1;

    if (!serverSocket.DoListen(15))
        return -1;
    while(true)
    {
        if (!serverSocket.DoAccept())
            return -1;
        while (true)
        {
            std::string recv_str;
            if(!serverSocket.Recv(recv_str))
            {
                break;
            }
            std::string send_str = "I have received: ";
            send_str += recv_str;
            if(!serverSocket.Send(send_str))
            {
                break;
            }
        }
        if(!serverSocket.DoShutdown())
        {
            return -1;
        }
    }

    return 0;
}

#include "ClientSocket.h"
#include <iostream>

int main(int argc, char* argv[])
{
    ClientSocket clientSocket;
    for (int i_connect = 0; i_connect < 3; i_connect++)
    {
        if (!clientSocket.DoInit())
            return -1;
        //if (!clientSocket.DoBind("127.0.0.1", 1235))
        //    return -1;
        if (!clientSocket.DoConnect("127.0.0.1", 1234, 10))
            return -1;
        for (int i = 0; i < 5; i++)
        {
            if(!clientSocket.Send("hello world! #" + std::to_string(i)))
            {
                break;
            }
            std::string recv_str;
            if(!clientSocket.Recv(recv_str))
            {
                break;
            }
            std::cout << "receive " << "string ("<< recv_str.size() <<") from server: " << recv_str << std::endl;
        }
        if(!clientSocket.DoShutdown())
        {
            return -1;
        }
    }
    system("pause");
    return 0;
}

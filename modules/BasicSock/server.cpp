#include <iostream>
#include <stdlib.h>
#include <WinSock2.h>
//#pragma comment(lib, "WS2_32.Lib")

#define DEFAULT_BUFLEN 512

int main(int argc, char* argv[])
{
	//初始化 DLL
	WSADATA wsaData;
	int wsa_err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsa_err != 0) {
		std::cout << "WSAStartup failed with error: " << wsa_err << std::endl;
		return -1;
	}

	//创建套接字
	SOCKET listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenfd == INVALID_SOCKET)
	{
		std::cout << "socket function failed with error: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return -1;
	}

	//绑定套接字
	sockaddr_in bindaddr;
	memset(&bindaddr, 0, sizeof(bindaddr));  //每个字节都用0填充
	bindaddr.sin_family = AF_INET;  //使用IPv4地址
	bindaddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
	//sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);  //0.0.0.0
	bindaddr.sin_port = htons(1234);  //端口
	if (bind(listenfd, (SOCKADDR*)&bindaddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		std::cout << "bind function failed with error: " << WSAGetLastError() << std::endl;
		if (closesocket(listenfd) == SOCKET_ERROR)
		{
			std::cout << "closesocket function failed with error: " << WSAGetLastError() << std::endl;
		}
		WSACleanup();
		return -1;
	}

	//进入监听状态
	if (listen(listenfd, SOMAXCONN) == SOCKET_ERROR)
	{
		std::cout << "listen function failed with error: " << WSAGetLastError() << std::endl;
		if (closesocket(listenfd) == SOCKET_ERROR)
		{
			std::cout << "closesocket function failed with error: " << WSAGetLastError() << std::endl;
		}
		WSACleanup();
		return -1;
	}

	while(true)
	{
		std::cout << "listening on socket ..." << std::endl;
		//接收客户端连接
		SOCKADDR clientaddr;
		int clientaddrlen = sizeof(clientaddr);
		SOCKET clientfd = accept(listenfd, (SOCKADDR*)&clientaddr, &clientaddrlen);
		if (clientfd == INVALID_SOCKET) 
		{
			std::cout << "accept function failed with error: " << WSAGetLastError() << std::endl;
			continue;
		}
		std::cout << "client connected." << std::endl;

		//接收客户端发来的数据
		char recvbuf[DEFAULT_BUFLEN];
		int recvbuflen = DEFAULT_BUFLEN;
		int n_recv;
		// Receive until the peer shuts down the connection
		do {
			n_recv = recv(clientfd, recvbuf, recvbuflen, NULL);
			if (n_recv > 0) {
				std::cout << "receive " << n_recv << " bytes from client." << std::endl;
				// Echo the buffer back to the sender
				char sendbuf[DEFAULT_BUFLEN] = "I have received: ";
				strcat(sendbuf, recvbuf);
				int n_send = strlen(sendbuf) + 1;// '\0' included
				n_send = send(clientfd, sendbuf, n_send, 0);
				if (n_send == SOCKET_ERROR) {
					std::cout << "send function failed with error: " << WSAGetLastError() << std::endl;
					break;
				}
				else
				{
					std::cout << "send " << n_send << " bytes to client." << std::endl;
				}
			}
			else if (n_recv == 0)
			{
				std::cout << "Connection closing ... \n" << std::endl;
			}
			else 
			{
				std::cout << "recv function failed with error: " << WSAGetLastError() << std::endl;
			}

		} while (n_recv > 0);

		// shutdown the connection since we're done
		if (shutdown(clientfd, SD_SEND) == SOCKET_ERROR) 
		{
			std::cout << "shutdown function failed with error: " << WSAGetLastError() << std::endl;
			closesocket(clientfd);
		}
		//关闭套接字
		closesocket(clientfd);
	}


	closesocket(listenfd);
	//终止 DLL 的使用
	if (WSACleanup() == SOCKET_ERROR)
	{
		std::cout << "WSACleanup function failed with error: " << WSAGetLastError() << std::endl;
		system("pause");
		return -1;
	}
	system("pause");
	return 0;
}
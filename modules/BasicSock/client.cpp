#include <iostream>
#include <stdlib.h>
#include <WinSock2.h>
//#pragma comment(lib, "WS2_32.Lib")

#define DEFAULT_BUFLEN 512


int main() {
	//初始化DLL
	WSADATA wsaData;
	int wsa_err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsa_err != 0) {
		std::cout << "WSAStartup failed with error: " << wsa_err << std::endl;
		return -1;
	}
	char sendbuf[DEFAULT_BUFLEN];
	char recvbuf[DEFAULT_BUFLEN];
	int n_send, n_recv;

	for (int i_connect = 0; i_connect < 3; i_connect++)
	{
		std::cout << "create socket #" << i_connect << std::endl;
		//创建套接字
		SOCKET clientfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (clientfd == INVALID_SOCKET) {
			std::cout << "socket function failed with error: " << WSAGetLastError() << std::endl;
			WSACleanup();
			return -1;
		}
		//向服务器发起请求
		sockaddr_in serveraddr;
		memset(&serveraddr, 0, sizeof(serveraddr));  //每个字节都用0填充
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
		serveraddr.sin_port = htons(1234);
		if (connect(clientfd, (SOCKADDR*)&serveraddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
		{
			std::cout << "connect function failed with error: " << WSAGetLastError() << std::endl;
			if (closesocket(clientfd) == SOCKET_ERROR)
			{
				std::cout << "closesocket function failed with error: " << WSAGetLastError() << std::endl;
			}
			clientfd = INVALID_SOCKET;
			WSACleanup();
			return -1;
		}
		std::cout << "connect to server." << std::endl;
		for (int i = 0; i < 5; i++)
		{
			sprintf(sendbuf, "hello world! #%d", i);
			n_send = strlen(sendbuf) + 1;
			n_send = send(clientfd, sendbuf, n_send, NULL);
			if (n_send == SOCKET_ERROR) {
				std::cout << "send function failed with error: " << WSAGetLastError() << std::endl;
				closesocket(clientfd);
				WSACleanup();
				return -1;
			}
			std::cout << "send " << n_send << " bytes to server: " << sendbuf << std::endl;
			//接收服务器传回的数据
			n_recv = DEFAULT_BUFLEN;
			n_recv = recv(clientfd, recvbuf, n_recv, NULL);
			if (n_recv > 0) {
				//输出接收到的数据
				std::cout << "receive " << n_recv << " bytes from server: " << recvbuf << std::endl;
			}
			else
			{
				//接收到0字节表示对方已关闭连接
				std::cout << "recv function failed with error: " << WSAGetLastError() << std::endl;
				break;
			}

		}

		// shutdown the send half of the connection since no more data will be sent
		if (shutdown(clientfd, SD_SEND) == SOCKET_ERROR) {
			std::cout << "shutdown function failed with error: " << WSAGetLastError() << std::endl;
			closesocket(clientfd);
			WSACleanup();
			return -1;
		}
		else
		{
			std::cout << "Connection closing ... \n" << std::endl;
		}
		//关闭套接字
		closesocket(clientfd);

	}
	//终止使用 DLL
	if (WSACleanup() == SOCKET_ERROR)
	{
		std::cout << "WSACleanup function failed with error: " << WSAGetLastError() << std::endl;
		system("pause");
		return -1;
	}
	system("pause");
	return 0;
}

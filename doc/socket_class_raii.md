# 封装网络库
在socket通信时需要频繁地判断操作是否成功，如果失败就进行断开连接、关闭socket、退出程序等操作。  
这种处理过于繁琐，实际上可以把服务器和客户端代码使用遵循RAII惯用法的class来实现，在构造函数中初始化并获取资源，在析构函数中释放资源。  
这样，在使用中创建相应的对象，当程序正常或异常退出时，该对象都会被析构从而将资源释放掉。

代码位于[modules/SocketRAII](../modules/SocketRAII)文件夹下.

```
modules\SocketRAII
|   CMakeLists.txt
|
+---ClientSocket
|       ClientSocket.cpp
|       ClientSocket.h
|       test_ClientSocket.cpp
+---ServerSocket
|       ServerSocket.cpp
|       ServerSocket.h
|       test_ServerSocket.cpp
+---build
\---install
```


# 编译运行
## 通过cmake生成VS工程
```
PS SocketRAII> mkdir build
PS SocketRAII> cd build
PS SocketRAII\build> cmake -G "Visual Studio 15 2017" -T v141 -A x64 ..
```
## 编译
在`SocketRAII/build`文件夹下双击打开`SocketRAII.sln`, 生成解决方案. 并单独生成`INSTALL`项目.
## 运行
在`SocketRAII/install/bin`文件夹下先后双击运行`test_ServerSocket.exe`和`test_ClientSocket.exe`.

`test_ClientSocket.exe`执行完成后可以按任意键退出运行窗口;   
`test_ServerSocket.exe`不会自动退出，而是一直等待客户端连接，然后与客户端通信. `test_ServerSocket.exe`只能通过手动关闭窗口来退出运行.


# 代码解析
## 断开重连
当客户端与服务器断开连接后，要再次恢复连接，不能使用之前的socket句柄，必须重建通过`SOCKET WSAAPI socket(int af,int type,int protocol)`函数创建新的句柄，用于连接与通信.  
使用我封装后的代码进行测试，通过`for`循环多次执行连接与断开连接的操作，在每次循环体中，要请求连接到服务器前，必须调用`ClientSocket::DoInit()`重新创建一个socket句柄，然后执行`ClientSocket::DoConnect()`连接到服务器.  
- https://github.com/siyouluo/SocketSamples/blob/30b9462e0edfb94330ca8e9d0e44a632572f5a65/modules/SocketRAII/ClientSocket/test_ClientSocket.cpp#L7-L14

## 失败重试
在服务器与客户端的通信测试中，一般需要服务器先运行起来，然后客户端才能申请连接上去. 但为了程序的鲁棒性，有必要测试一下当服务器没有运行起来时，先运行客户端的表现.

经过测试，单独运行客户端时，客户端通过`connect`函数申请连接到服务器，但此时服务器并不在运行，一段时间后`connect`函数返回，并通过`WSAGetLastError()`读取到错误码为`WSAECONNREFUSED(10061)`, [官方文档](https://docs.microsoft.com/en-us/windows/win32/winsock/windows-sockets-error-codes-2)解释为
```
Connection refused.
No connection could be made because the target computer actively refused it. 
This usually results from trying to connect to a service that is inactive on the foreign host
—that is, one with no server application running.
```
错误码的解释与实际情况是一致的, 即尝试连接到一个不在运行的服务器.

按理说，在阻塞模式下，`connect`函数会一直阻塞在此处不断尝试连接到服务器，因此有时候需要使用非阻塞方式的客户端.  

但实际测试下，`connect`连接失败就直接返回了, 没有将程序长时间阻塞在此处，这正合我意，就没有必要自寻烦恼了.
当然，也可以稍作修改，通过用户指定尝试次数，让客户端连接失败后就再多试几次，等待服务器上线.  
- https://github.com/siyouluo/SocketSamples/blob/0c57337561b7f3d2c0a056c536b3e5c349c2611f/modules/SocketRAII/ClientSocket/test_ClientSocket.cpp#L13
- https://github.com/siyouluo/SocketSamples/blob/0c57337561b7f3d2c0a056c536b3e5c349c2611f/modules/SocketRAII/ClientSocket/ClientSocket.h#L15
- https://github.com/siyouluo/SocketSamples/blob/0c57337561b7f3d2c0a056c536b3e5c349c2611f/modules/SocketRAII/ClientSocket/ClientSocket.cpp#L72-L84


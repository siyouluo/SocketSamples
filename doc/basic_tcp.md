# Basic TCP

本项目实现简单的socket服务器与客户端连接与通信，代码位于[modules/BasicSock](../modules/BasicSock)文件夹下.
```
MODULES\BASICSOCK
│  CMakeLists.txt
│  client.cpp
│  server.cpp
│
├─build
└─install
```


# 编译运行
## 通过cmake生成VS工程
```
PS BasicSock> mkdir build
PS BasicSock> cd build
PS BasicSock\build> cmake -A x64 ..
```
## 编译
在`BasicSock/build`文件夹下双击打开`BasicSock.sln`, 生成解决方案. 并单独生成`INSTALL`项目.
## 运行
在`BasicSock/install/bin`文件夹下先后双击运行`server.exe`和`client.exe`.

`client.exe`执行完成后可以按任意键退出运行窗口;   
`server.exe`不会自动退出，而是一直等待客户端连接，然后与客户端通信. `server.exe`只能通过手动关闭窗口来退出运行.

# 代码解析
## 代码总体流程
服务器端启动监听后，开启第一层`while(true)`循环，通过阻塞模式的`accept()`函数等待客户端连接, 连接成功后进入第二层循环，不断接受客户端发来的数据，在数据前面添加一个字符串，再发送回客户端. 直到`发送失败/接收失败/对方关闭了连接`才退出第二层循环并关闭与当前客户端的连接，重新回到第一层循环等待新的客户端连接.

客户端代码启动后先进入第一层循环，每次循环都创建一个新的客户端`socket`，然后与服务器连接，连接成功后进入第二层循环(共5次循环)，不断往服务器发送字符串，并接收服务器返回的数据，完成第二层循环的全部发送与接收任务后，通过`shutdown()`关闭与服务器的连接，并通过`closesocket()`关闭当前客户端客户端`socket`. 然后回到第一层循环，创建新的客户端`socket`与服务器连接并通信.

客户端`socket`在关闭连接后是无法再重新恢复连接的，只能创建新的`socket`才能恢复与服务器的连接与通信.

## 链接库`WS2_32.Lib`
windows下socket编程需要链接系统静态库`WS2_32.Lib`, 因此有些程序中会在代码段之前添加如下形式的编译指令(文件大小写不重要)
```cpp
#pragma comment(lib, "ws2_32.lib") 
```
并且因为`comment`第一个参数指定了`lib`, 甚至可以写成
```cpp
#pragma comment(lib, "ws2_32") 
```
但`#pragma`指令实际上只有在`MSVC`编译器中才可以使用，而在使用`gcc`进行编译时是无效的.

因此最好还是在工程属性中配置链接库,而不是在代码文件中指定.


## PF_INET or AF_INET

在调用`socket/bind/connect`三个函数时会用到`PF_INET/AF_INET`这两个宏，而且在许多代码中是不加区分随意使用的.

> 其实是TCP/IP的设计者一开始想多了。
> PF是protocol family，AF是address family，作者一开始以为可能某个协议族有多种形式的地址，所以在API上把它们分开了，创建socket用PF，bind/connect用AF。结果一个PF只有一个AF，从来没有过例外，所以就混用了。

在`WinSock2.h`定义了许多`PF`相关的宏，取值与相应的`AF`一致.可以看到`PF_INET`与`AF_INET`就是一致的.

```cpp
/* WinSock2.h */
/*
 * Protocol families, same as address families for now.
 */
#define PF_UNSPEC       AF_UNSPEC
#define PF_UNIX         AF_UNIX
#define PF_INET         AF_INET
...
```


```cpp
/* ws2def.h */
#define AF_UNSPEC       0               // unspecified
#define AF_UNIX         1               // local to host (pipes, portals)
#define AF_INET         2               // internetwork: UDP, TCP, etc.
#define AF_IMPLINK      3               // arpanet imp addresses
...
```


- [关于PF_INET和AF_INET的区别](https://blog.csdn.net/xiongmaojiayou/article/details/7584211)
- [PF_INET AF_INET的区别是什么？](https://blog.csdn.net/xiaolei251990/article/details/83030523?spm=1001.2101.3001.6661.1&utm_medium=distribute.pc_relevant_t0.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-1.pc_relevant_default&depth_1-utm_source=distribute.pc_relevant_t0.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-1.pc_relevant_default&utm_relevant_index=1)
- [Unix网络编程 卷1 套接字联网API(第三版) 4.2 socket函数](https://github.com/unpbook/unpv13e)

# 参考
- [一个简单的Windows下的socket程序 - C语言中文网](http://c.biancheng.net/cpp/html/3031.html)
- [Windows Sockets API](https://docs.microsoft.com/en-us/windows/win32/api/_winsock/)
- [winsock2.h API](https://docs.microsoft.com/en-us/windows/win32/api/winsock2/)
- [Getting Started with Winsock](https://docs.microsoft.com/en-us/windows/win32/winsock/getting-started-with-winsock)
- [Running the Winsock Client and Server Code Sample](https://docs.microsoft.com/en-us/windows/win32/winsock/finished-server-and-client-code)
- [Complete Winsock Client Code](https://docs.microsoft.com/en-us/windows/win32/winsock/complete-client-code)
- [Complete Winsock Server Code](https://docs.microsoft.com/en-us/windows/win32/winsock/complete-server-code)
- [Windows Sockets Error Codes](https://docs.microsoft.com/en-us/windows/win32/winsock/windows-sockets-error-codes-2)

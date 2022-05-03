# Socket 网络编程
为了实现对工业机器人的控制，需要实现一个windows下的socket客户端，通过局域网连接，与运行在机器人主机上的服务器进行通信。本项目借此系统地整理学习一下socket网络编程相关知识。  
为了方便，本项目大多数代码运行在windows平台上，同时部分代码会在linux系统上进行测试(因为实际使用的机器人控制系统是基于linux平台的开发的)

**环境:**  
- `Intel x64`
- `Windows 10`
- `Visual Studio Community 2017`
- `CMake 3.18.5`
- `winsock2`

# Socket 简介
> 80年代初，美国政府的高级研究工程机构（ARPA）给加利福尼亚大学Berkeley分校提供了资金，让他们在UNIX操作系统下实现TCP/IP协议。在这个项目中，研究人员为TCP/IP网络通信开发了一个API（应用程序接口）。这个API称为Socket接口（套接字）。今天，SOCKET接口是TCP/IP网络最为通用的API，也是在INTERNET上进行应用开发最为通用的API。

> 90年代初，由Microsoft联合了其他几家公司共同制定了一套WINDOWS下的网络编程接口，即WindowsSockets规范。它是BerkeleySockets的重要扩充，主要是增加了一些异步函数，并增加了符合Windows消息驱动特性的网络事件异步选择机制。WindowsSockets规范是一套开放的、支持多种协议的Windows下的网络编程接口。从1991年的1.0版到1995年的2.0.8版，经过不断完善并在Intel、Microsoft、Sun、SGI、Informix、Novell等公司的全力支持下，已成为Windows网络编程的事实上的标准。目前，在实际应用中的WindowsSockets规范主要有1.1版和2.0版。两者的最重要区别是1.1版只支持TCP/IP协议，而2.0版可以支持多协议。2.0版有良好的向后兼容性，任何使用1.1版的源代码，二进制文件，应用程序都可以不加修改地在2.0规范下使用。

> SOCKET实际在计算机中提供了一个通信端口，可以通过这个端口与任何一个具有SOCKET接口的计算机通信。应用程序在网络上传输，接收的信息都通过这个SOCKET接口来实现。在应用开发中就像使用文件句柄一样，可以对SOCKET句柄进行读，写操作。

# Windows SDK
> 微软每推出一个重要的windows版本，一般都会同时推出一个SDK（Software Development Kit）。SDK包含了开发该windows版本所需的windows函数和常数定义、API函数说明文档、相关工具和示例。SDK一般使用C语言，但不包括编译器。

在安装`Visual Studio`时一般同时安装了windows sdk, 例如`Windows 10 SDK (10.0.17763.0)`, `Windows 8.1 SDK`等.

`Windows SDK`中包含了网络通信的相关头文件和库函数，例如`Windows.h`, `winsock.h`, `WinSock2.h`, `ws2_32.dll`, `WS2_32.Lib`等.

# WinSock2

- [WinSock 2 Information](http://www.sockets.com/winsock2.htm)

在windows下进行socket开发优先使用`WinSock2`, 需要在程序中引入如下依赖文件:  
- 头文件: `WinSock2.h`, `Windows.h`
- 静态链接库: `WS2_32.Lib`
- 动态链接库: `ws2_32.dll`  

**注意:**   
头文件`WinSock2.h`在整个工程中的引入顺序必须先于`Windows.h`, 因为查看`Windows.h`发现其中有如下代码段, 当`WIN32_LEAN_AND_MEAN`和`_MAC`都没有被定义时，将会引入`winsock.h`头文件. 

- `Windows.h`   

```cpp {.line-numbers}
// Windows.h
#ifndef WIN32_LEAN_AND_MEAN
#ifndef _MAC
#include <winperf.h>
#include <winsock.h>
#endif
#endif /* WIN32_LEAN_AND_MEAN */
```

- `WinSock2.h`  

```cpp {.line-numbers}
// WinSock2.h
#ifndef _WINSOCK2API_
#define _WINSOCK2API_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
...
/*
 * Pull in WINDOWS.H if necessary
 */
#ifndef _INC_WINDOWS
#include <windows.h>
#endif /* _INC_WINDOWS */
...
#endif  /* _WINSOCK2API_ */
```

- `winsock.h`  

```cpp {.line-numbers}
// winsock.h
#include <winapifamily.h>

#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
...
#endif  /* _WINSOCKAPI_ */
```

头文件`winsock.h`是用于`winsock 1.x`的，其中部分内容与`WinSock2.h`存在重复定义的问题, 二者不能很好的共存，`WinSock2.h`是被设计用于替换`winsock.h`的，而非补充它. 

当`winsock.h`和`WinSock2.h`共存时，编译器往往会报出如下`重定义`错误:  
```
warning C4005: “AF_IPX”: 宏重定义
warning C4005: “AF_MAX”: 宏重定义
warning C4005: “SO_DONTLINGER”: 宏重定义
error C2011: “sockaddr_in”:“struct”类型重定义
error C2143: 语法错误 : 缺少“}”(在“常量”的前面)
error C2143: 语法错误 : 缺少“;”(在“常量”的前面) 
```

为了解决这个问题，需要保证在整个工程中，头文件`WinSock2.h`引入顺序必须先于`Windows.h`. 这种情况下，`WinSock2.h`中定义的`_WINSOCKAPI_`宏将会阻止编译器去处理`winsock.h`中的相关内容. 

但是在稍微大型的工程都存在较多的嵌套`include`, 特别是在使用第三方开发的库函数时，其中可能调用了`Windows.h`而自己并不知情. 最终不知不觉搞混了这两个头文件的引入顺序. 因此在编程中有如下做法可供参考: 
- 保证`WinSock2.h`在所有其他头文件的前面, 不只是`Windows.h`;
- 由于`WinSock2.h`中引入了`Windows.h`, 开发者可以不用主动引入`Windows.h`;
- 在每次引入`Windows.h`之前都添加`include <WinSock2.h>`;
- 在预处理器定义中添加`WIN32_LEAN_AND_MEAN`, 保证`winsock.h`绝对不会被`Windows.h`引入;
    例如`Visual Studio`中: `配置属性`->`C/C++`->`预处理器`->`预处理器定义`第一行添加`WIN32_LEAN_AND_MEAN`.


**参考:**  
- [can't include windows.h before winsock2.h - stackoverflow](https://stackoverflow.com/questions/9153911/is-there-a-difference-between-winsock-h-and-winsock2-h/9168850#9168850)
- [error C2011: “sockaddr”:“struct”类型重定义错误 ----解决办法](https://blog.csdn.net/ccnu027cs/article/details/103593923?utm_medium=distribute.pc_relevant.none-task-blog-baidujs_baidulandingword-1&spm=1001.2101.3001.4242)
- [WinSock1.1和WinSock2.0 - 博客园](https://www.cnblogs.com/MakeView660/p/9237990.html)

# 项目例程
- [基础TCP/IP通信](doc/basic_tcp_ip.md)
- [封装TCP/IP通信代码](doc/object_oriented_tcp_ip.md)
- [非阻塞的TCP/IP客户端](doc/non_block_tcp_ip.md)

# 参考教程

- [C/C++ socket编程教程 - C语言中文网](http://c.biancheng.net/cpp/socket/)
- [Socket编程指南及示例程序](http://www.blogjava.net/wxb_nudt/archive/2007/11/01/157623.html)


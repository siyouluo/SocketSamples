# 非阻塞socket连接
- 阻塞模式: 当某个函数执行成功的条件当前不满足时，该函数会阻塞当前执行线程，程序执行流在超时时间到达或执行成功的条件满足后恢复继续执行。
- 非阻塞模式: 即使某个函数执行成功的条件当前不能满足，该函数也不会阻塞当前执行线程，而是立即返回，继续执行程序流。



当socket使用阻塞模式时，connect函数会一直到有明确的结果才会返回（或连接成功或连接失败），如果服务器地址“较远”或者网络状况不好，连接速度较慢，则程序可能会在 connect 函数处阻塞好一会儿（如两三秒之久）。
虽然这一般也不会对依赖于网络通信的程序造成什么影响，但在实际项目中，我们一般倾向于使用异步connect技术（非阻塞connect），一般有如下步骤：  
1. 创建socket，将socket设置为非阻塞模式。
2. 调用 connect 函数，此时无论 connect 函数是否连接成功，都会立即返回；如果返回-1，则并不一定表示连接出错，如果此时错误码是`WSAEINPROGRESS`，则表示正在尝试连接。
3. 调用select函数，在指定的时间内判断该socket是否可写，如果可写，则说明连接成功，反之认为连接失败。


默认创建的socket都是阻塞模式的, 本节尝试建立一个非阻塞模式的socket客户端，且仅在connect阶段将其设置为非阻塞模式，并设置超时参数和重复尝试次数，直到连接成功或者失败太多次数后直接退出.


代码位于[modules/NonBlockSock](../modules/NonBlockSock)文件夹下.

```
modules\NonBlockSock
|   CMakeLists.txt
|
+---ClientSocket
|       ClientSocket.cpp
|       ClientSocket.h
|       test_ClientSocket.cpp
+---build
\---install
```

服务器端程序可直接使用[modules/SocketRAII](../modules/SocketRAII)中的`ServerSocket`来辅助测试.

# 编译运行
## 通过cmake生成VS工程
```
PS NonBlockSock> mkdir build
PS NonBlockSock> cd build
PS NonBlockSock\build> cmake -G "Visual Studio 15 2017" -T v141 -A x64 ..
```
## 编译
在`NonBlockSock/build`文件夹下双击打开`NonBlockSock.sln`, 生成解决方案. 并单独生成`INSTALL`项目.
## 运行
在`NonBlockSock/install/bin`文件夹下先双击运行`test_ClientSocket.exe`, 待客户端尝试连接两三次失败后，再运行服务器端.

# 代码解析

# 参考
- [select function (winsock2.h) - docs.microsoft](https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-select)
- [Winsocket 三：非阻塞server&client程序（tcp）](https://blog.csdn.net/u013071074/article/details/26354781)
- [Winsocket入门教程二：非阻塞式服务器和客户端程序(TCP)](https://blog.csdn.net/shining100/article/details/5651878)
- [非阻塞式服务器和客户端程序(TCP)【简单的原理例子】](https://www.cnblogs.com/linmzh/archive/2013/01/15/2860704.html)
- [面向连接的socket数据处理过程以及非阻塞connect问题](https://www.cnblogs.com/lidabo/p/4289360.html)


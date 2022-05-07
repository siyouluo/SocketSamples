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
PS SocketRAII\build> cmake -A x64 ..
```
## 编译
在`SocketRAII/build`文件夹下双击打开`SocketRAII.sln`, 生成解决方案. 并单独生成`INSTALL`项目.
## 运行
在`SocketRAII/install/bin`文件夹下先后双击运行`test_ServerSocket.exe`和`test_ClientSocket.exe`.

`test_ClientSocket.exe`执行完成后可以按任意键退出运行窗口;   
`test_ServerSocket.exe`不会自动退出，而是一直等待客户端连接，然后与客户端通信. `test_ServerSocket.exe`只能通过手动关闭窗口来退出运行.






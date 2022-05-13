# WinSock2网络编程
本项目的主要开发模式是  
1. 使用`VS Code`进行文本编辑, 包括代码编辑与文档整理;
2. 使用`CMake`配置项目，生成`Visual Studio`工程;
3. 在`Visual Studio`上进行编译，生成可执行文件、静态库，并安装到指定路径;

# 关于文件编码格式
在`Visual Studio`中的代码文件编码格式仅支持`GB2312`和`UTF-8 with BOM`, 不支持`UTF-8`. 
但由于个人偏好问题，我的代码都是使用`UTF-8`编码的，如果直接通过`Visual Studio`进行编译，可能会报出如下警告.  
```txt
warning C4819: 该文件包含不能在当前代码页(936)中表示的字符
```
编码格式问题有时甚至会导致`error`, 并且输出的错误信息与实际文件驴唇不对马嘴.

要解决这个问题，可以给`msvc`添加编译选项`/source-charset:utf-8`,让编译器知道编码字符集为`utf-8`.   
- 如果是从`CMake`生成VS工程，可以在`CMakeLists.txt`中添加:
    - https://github.com/siyouluo/SocketSamples/blob/2ccd22cd1407b8186dfbcf218147fa6c0bbe7d28/modules/BasicSock/CMakeLists.txt#L9-L15
- 如果是直接在VS中新建工程，可以执行: `右键项目`->`属性`->`配置属性`->`C/C++`->`命令行`->`其他选项`->输入`%(AdditionalOptions) /source-charset:utf-8`

**参考:**  
- [vscode 中 warning C4819: 该文件包含不能在当前代码页(936)中表示的字符 - CSDN](https://blog.csdn.net/qq_38026359/article/details/107188429)

# WinSock2编程

- [WinSock 2 Information](http://www.sockets.com/winsock2.htm)
- [WinSock2 API](https://docs.microsoft.com/en-us/windows/win32/api/winsock2/)

`WinSock2`是Windows SDK中用于socket网络通信的库, 在Windows平台上进行socket编程，优先选择`WinSock 2`，而非`WinSock 1.1`. 

要使用`WinSock2`进行开发，主要用到如下文件:    
- 头文件: `WinSock2.h`
- 静态链接库: `WS2_32.Lib`
- 动态链接库: `ws2_32.dll`  

## 文件名大小写 
一般情况下, `windows`平台上文件和路径名不区分大小写，因此许多地方在引入头文件或者库文件时，使用的文件名与实际的大小写并不一致，这在windows上是可行的. 
而在`linux`系统上通常文件名是大小写敏感的，必须与实际一致.

更确切地说, 是否`大小写敏感`取决于文件系统，在windows上是可选的，而大多数unix系统上的文件系统是大小写敏感的.   
- https://stackoverflow.com/a/1951969 

## 版本冲突: `WinSock 2`和`WinSock 1.1`
### 避免引入`winsock.h`
`WinSock 2`是用来替代`WinSock 1.1`的，但如果在一个项目里同时使用了这两个版本的socket库，则会产生冲突，因为它们各自的头文件`WinSock2.h`和`winsock.h`中存在一些同名的定义，如果被引入到了同一个工程中，编译器往往会报出如下`重定义`错误:  
```
warning C4005: “AF_IPX”: 宏重定义
warning C4005: “AF_MAX”: 宏重定义
warning C4005: “SO_DONTLINGER”: 宏重定义
error C2011: “sockaddr_in”:“struct”类型重定义
error C2143: 语法错误 : 缺少“}”(在“常量”的前面)
error C2143: 语法错误 : 缺少“;”(在“常量”的前面) 
```

因此在使用`WinSock 2`进行网络编程时应该避免引入`winsock.h`头文件.

### 通过宏定义来排除`winsock.h`
在`WinSock2.h`中存在定义了一个宏`_WINSOCKAPI_`, 而`winsock.h`中的内容只有在该宏没有被定义时才会生效. 
因此即使在一个工程中同时引入了`WinSock2.h`和`winsock.h`, 只要`WinSock2.h`先于`winsock.h`被引入就可以避免二者发生冲突.

- https://github.com/siyouluo/SocketSamples/blob/90391c4a9c05948573582ba7af664a3a7d20223f/doc/WindowsSDK/WinSock2.h#L17

- https://github.com/siyouluo/SocketSamples/blob/90391c4a9c05948573582ba7af664a3a7d20223f/doc/WindowsSDK/winsock.h#L16-L17

### 避免间接引入`winsock.h`
有时即使我们没有主观地引入`winsock.h`，也会发生版本冲突的问题. 
这主要是因为另一个`Windows SDK`的头文件`Windows.h`间接引入了`winsock.h`.
当`WIN32_LEAN_AND_MEAN`和`_MAC`都没有被定义时，将会引入`winsock.h`头文件. 

- https://github.com/siyouluo/SocketSamples/blob/90391c4a9c05948573582ba7af664a3a7d20223f/doc/WindowsSDK/Windows.h#L182-L197

为了解决这个问题，头文件`WinSock2.h`在整个工程中的引入顺序必须先于`Windows.h`.


而对于稍微大型的工程都存在较多的嵌套`include`, 特别是在使用第三方开发的库函数时，其中可能调用了`Windows.h`而自己并不知情. 最终不知不觉地在`WinSock2.h`之前引入了`Windows.h`，从而间接引入了`winsock.h`导致版本冲突. 

因此在编程中有如下准则可供参考: 
- 保证`WinSock2.h`在所有其他头文件的前面, 不只是`Windows.h`;
- 由于`WinSock2.h`中引入了`Windows.h`, 开发者可以不用主动引入`Windows.h`;
- 在每次引入`Windows.h`之前都添加`#include <WinSock2.h>`;

### 彻底避免`Windows.h`引入`winsock.h`
查看`Windows.h`可发现只有在`WIN32_LEAN_AND_MEAN`没有被定义时才会引入`winsock.h`，因此我们可以手动定义该宏，来阻止`winsock.h`被引入. 可以通过如下两种方式进行宏定义，二者选其一即可.  

- 在`CMakeLists.txt`中进行配置:  
    - https://github.com/siyouluo/SocketSamples/blob/2ccd22cd1407b8186dfbcf218147fa6c0bbe7d28/modules/BasicSock/CMakeLists.txt#L26
- 在`Visual Studio`中进行配置: `配置属性`->`C/C++`->`预处理器`->`预处理器定义`第一行添加`WIN32_LEAN_AND_MEAN`.

通过定义`WIN32_LEAN_AND_MEAN`可以精简`Window.h`头文件的大小，定义后会去除掉`Window.h`中一些不那么常用的API,例如`Cryptography`, `DDE`, `RPC`, `Shell`, `Windows Sockets`.  

**参考:**  
- [can't include windows.h before winsock2.h - stackoverflow](https://stackoverflow.com/questions/9153911/is-there-a-difference-between-winsock-h-and-winsock2-h/9168850#9168850)
- [Faster Builds with Smaller Header Files - docs.microsoft](https://docs.microsoft.com/en-us/windows/win32/winprog/using-the-windows-headers#faster-builds-with-smaller-header-files)
- [The WIN32_LEAN_AND_MEAN macro prevents the Winsock.h from being included by the Windows.h header](https://docs.microsoft.com/en-us/windows/win32/winsock/creating-a-basic-winsock-application)
- [error C2011: “sockaddr”:“struct”类型重定义错误 ----解决办法](https://blog.csdn.net/ccnu027cs/article/details/103593923?utm_medium=distribute.pc_relevant.none-task-blog-baidujs_baidulandingword-1&spm=1001.2101.3001.4242)
- [WinSock1.1和WinSock2.0 - 博客园](https://www.cnblogs.com/MakeView660/p/9237990.html)




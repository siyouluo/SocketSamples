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

# 代码解析


# 参考
- [一个简单的Windows下的socket程序 - C语言中文网](http://c.biancheng.net/cpp/html/3031.html)

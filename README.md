# Example程序同时支持Win32Desktop和PosixLinuxGlibc两个平台

* BuildAll/Solution/Win32Desktop/PTRuntime_Win32Desktop.sln
* BuildAll/Solution/PosixLinuxGlibc/PTRuntime_PosixLinuxGlibc.sln

# Runtime程序同时支持Win32Desktop、Win32Runtime、PosixLinuxGlibc和PosixLinuxBionic四个平台

* BuildAll/Solution/Win32Runtime/PTRuntime_Win32Runtime.sln
* BuildAll/Solution/PosixLinuxBionic/PTRuntime_PosixLinuxBionic.sln

# LinuxGlibc和LinuxBionic分别是指什么？

CentOS和Android都是Linux发行版，但是CentOS上编译生成的二进制文件却不能在Android上直接运行，这是为什么呢？

因为Linux内核只提供了PosixAPI层的部分功能，其余功能由C运行时库提供，操作系统在PosixAPI层的行为由Linux内核和C运行时库共同决定。

相同C运行时库的操作系统中的二进制文件是可以通用的。

理论上，如果能基于Bionic编译出CentOS上除Linux内核和Glibc以外的所有软件（包括但不限于：桌面环境，C++运行时库），再将Android系统中除Linux内核和Glibc以外的所有软件（包括但不限于：桌面环境，C++运行时库，系统APP）复制到CentOS上，那么AndroidAPK文件可以直接在CentOS上运行。（实际中需要考虑到应用程序层面的因素，比如一些应用程序在检测不到IMEI码的时候会自动崩溃或者依赖于特定目录的配置）

比如，我们熟悉的"动态链接库"就是C运行时库提供的功能，操作系统中"动态链接库"相关的行为由C运行时库决定。

当应用程序启动时，Glibc版本的操作系统由/lib/ld-linux.so.2加载动态链接库，而Bionic版本的操作系统由/system/bin/linker加载动态链接库，可以用"readelf -d"查看Executable文件的动态链接库加载器。

Glibc版本的操作系统在加载动态链接库时会依赖LD_LIBRARY_PATH环境变量，还可能发生全局符号介入的问题，而Bionic版本的操作系统并没有以上行为。



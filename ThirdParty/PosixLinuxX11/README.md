## Bionic-based Linux  
[bionic](https://android.googlesource.com/platform/bionic)  

由于glibc和stdc++的版本在不同Linux发行版之间差异较大，在一个Linux发行版上编译的二进制文件很难在未经重新编译的情况下在另一个Linux发行版上稳定并且行为一致地运行（Linux上的软件都倾向于开源，开发者们普遍期望最终用户下载源代码后在本地编译安装(著名的make install命令)，而很少考虑提供预编译的二进制文件供最终用户使用）  

因此，我们考虑到使用Android的bionic(-android-7.0.0_r36)，从而可以确保在一次编译后可以在任何的Linux发行版上稳定并且行为一致地运行  

参考google提供的官方文档[Building Open Source Projects Using Standalone Toolchains](https://developer.android.com/ndk/guides/standalone_toolchain#building_open_source_projects_using_standalone_toolchains)，我们基于bionic(-ndk-r11c)重新编译了PatriotEngine所依赖的所有第三方库（[X11](X11/README.md)等)  

同时，使用Android的bionic还带来了一个潜在的好处：我们可以复用Android的工具链(ndk-build等)，理论上，Linux可以和Android共用同一套编译系统，大大减少了开发时间  
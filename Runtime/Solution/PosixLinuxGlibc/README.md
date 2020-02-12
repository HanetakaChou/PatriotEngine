* 1\. [install the .NET Core SDK](https://docs.microsoft.com/en-us/dotnet/core/install/linux-package-manager-centos7)  
```
sudo rpm -Uvh https://packages.microsoft.com/config/centos/7/packages-microsoft-prod.rpm
sudo yum install dotnet-sdk-3.1
```
* 2\. the MSBuild is redistributed with the .NET Core
```
dotnet /usr/share/dotnet/sdk/3.1.101/MSBuild.dll
```
or
```
dotnet build
```
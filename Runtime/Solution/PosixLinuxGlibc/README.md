* 1\. [Add the Mono repository to your system](https://www.mono-project.com/download/stable/#download-lin-centos)  
```
rpmkeys --import "http://pool.sks-keyservers.net/pks/lookup?op=get&search=0x3fa7e0328081bff6a14da29aa6a19b38d3d831ef"
su -c 'curl https://download.mono-project.com/repo/centos7-stable.repo | tee /etc/yum.repos.d/mono-centos7-stable.repo'
```
* 2\. Install MSBuild
```
yum install msbuild
```
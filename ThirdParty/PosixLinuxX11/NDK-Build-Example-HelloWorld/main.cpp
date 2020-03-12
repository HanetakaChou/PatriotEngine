#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <iostream>

#include <link.h>
#include <dlfcn.h>

int main()
{
  void *handle = ::dlopen("libc.so", RTLD_NOW | RTLD_LOCAL);

  void *addr = ::dlsym(handle, "read");

  Dl_info info;
  int rc = dladdr(addr, &info);

  std::cout << info.dli_fname << std::endl;

  return 0;
}

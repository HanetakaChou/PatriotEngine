#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <iostream>

int main()
{
  char *buf;
  size_t buf_size;
  struct passwd pwd, *result;

  buf_size = sysconf(_SC_GETPW_R_SIZE_MAX);
  if (buf_size == -1)
    buf_size = 512;

  buf = static_cast<char *>(::malloc(buf_size));

  ::getpwuid_r(::getuid(), &pwd, buf, buf_size, &result);

  if (result)
  {
    std::cout << pwd.pw_dir << std::endl;
  }

  ::free(buf);

  std::cout << ::getenv("HOME") << std::endl;

  return 0;
}

#include <stdint.h>

extern "C" void backtrace_startup();
extern "C" void backtrace_shutdown();
extern "C" size_t backtrace_get(uintptr_t *frames, size_t frame_count);
extern "C" void backtrace_log(const uintptr_t *frames, size_t frame_count);

int main2()
{
  backtrace_startup();
  uintptr_t frames[128];
  size_t frame_count = backtrace_get(frames, 128);
  backtrace_log(frames, frame_count);
  backtrace_shutdown();
  return 0;
}

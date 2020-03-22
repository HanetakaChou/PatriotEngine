/*
 * Copyright (C) 2012 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <inttypes.h>
#include <malloc.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <unwind.h>
#include <dlfcn.h>

#include "MapData.h"
#include "backtrace.h"

#include <string>

#if defined(__LP64__)
#define PAD_PTR "016" PRIxPTR
#else
#define PAD_PTR "08" PRIxPTR
#endif

static MapData g_map_data;
static const MapEntry *g_current_code_map = NULL;

static _Unwind_Reason_Code trace_function(struct _Unwind_Context *context, void *arg);

void backtrace_startup()
{
  uintptr_t ip = reinterpret_cast<uintptr_t>(&trace_function);
  g_current_code_map = g_map_data.find(ip, NULL);
}

void backtrace_shutdown()
{
}

struct stack_crawl_state_t
{
  uintptr_t *frames;
  size_t frame_count;
  size_t cur_frame = 0;

  stack_crawl_state_t(uintptr_t *frames, size_t frame_count) : frames(frames), frame_count(frame_count)
  {
  }
};

static _Unwind_Reason_Code trace_function(struct _Unwind_Context *context, void *arg)
{
  stack_crawl_state_t *state = static_cast<stack_crawl_state_t *>(arg);

  uintptr_t ip = _Unwind_GetIP(context);

  // The instruction pointer is pointing at the instruction after the return
  // call on all architectures.
  // Modify the pc to point at the real function.
  if (ip != 0)
  {
#if defined(__arm__)
    // If the ip is suspiciously low, do nothing to avoid a segfault trying
    // to access this memory.
    if (ip >= 4096)
    {
      // Check bits [15:11] of the first halfword assuming the instruction
      // is 32 bits long. If the bits are any of these values, then our
      // assumption was correct:
      //  b11101
      //  b11110
      //  b11111
      // Otherwise, this is a 16 bit instruction.
      uint16_t value = (*reinterpret_cast<uint16_t *>(ip - 2)) >> 11;
      if (value == 0x1f || value == 0x1e || value == 0x1d)
      {
        ip -= 4;
      }
      else
      {
        ip -= 2;
      }
    }
#elif defined(__aarch64__)
    // All instructions are 4 bytes long, skip back one instruction.
    ip -= 4;
#elif defined(__i386__) || defined(__x86_64__)
    // It's difficult to decode exactly where the previous instruction is,
    // so subtract 1 to estimate where the instruction lives.
    ip--;
#endif
    if (ip != 0)
    {
      // Do not record the frames that fall in our own shared library.
      MapEntry pc_entry(ip);
      if ((g_current_code_map != NULL) && (!(pc_entry < (*g_current_code_map) || (*g_current_code_map) < pc_entry)))
      {
        return _URC_NO_REASON;
      }
      else
      {
        state->frames[state->cur_frame++] = ip;
        return (state->cur_frame >= state->frame_count) ? _URC_END_OF_STACK : _URC_NO_REASON;
      }
    }
    else
    {
      return _URC_NO_REASON;
    }
  }
  else
  {
    return _URC_NO_REASON;
  }
}

size_t backtrace_get(uintptr_t *frames, size_t frame_count)
{
  stack_crawl_state_t state(frames, frame_count);
  _Unwind_Backtrace(trace_function, &state);
  return state.cur_frame;
}

//#include <cxxabi.h>
extern "C" char *__cxa_demangle(const char *, char *, size_t *, int *);

bool backtrace_resolve(uintptr_t frame, uintptr_t *addr2line_addr, char *filename, size_t filename_max, char *symbolname, size_t symbolname_max, uintptr_t *symbol_offset)
{
  char const *symbol;
  uintptr_t offset;
  uintptr_t rel_pc;
  char const *soname;
  {
    Dl_info info;
    int ret_dl = dladdr(reinterpret_cast<void *>(frame), &info);
    if (ret_dl != 0)
    {
      symbol = info.dli_sname;
      offset = reinterpret_cast<uintptr_t>(info.dli_saddr);
    }
    else
    {
      symbol = NULL;
    }

    MapEntry const *entry = g_map_data.find(frame, &rel_pc);
    if (entry != NULL)
    {
      soname = entry->name();
    }
    else
    {
      if (ret_dl != 0)
      {
        rel_pc = offset;
        soname = info.dli_fname;
      }
    }

    if (soname == NULL)
    {
      soname = "<unknown>";
    }
  }

  //out -------------------------------
  if (addr2line_addr != NULL)
  {
    (*addr2line_addr) = rel_pc;
  }

  if (filename != NULL)
  {
    strncpy(filename, soname, filename_max);
  }

  if (symbol != NULL)
  {
    if (symbolname != NULL)
    {
      char *demangled_symbol = __cxa_demangle(symbol, NULL, NULL, NULL);
      char const *best_name = (demangled_symbol != NULL) ? demangled_symbol : symbol;
      strncpy(symbolname, best_name, symbolname_max);
      free(demangled_symbol);
    }

    if (symbol_offset != NULL)
    {
      (*symbol_offset) = (frame - offset);
    }
  }

  return (symbol != NULL);
}

std::string backtrace_string(const uintptr_t *frames, size_t frame_count)
{
  std::string str;

  for (size_t frame_num = 0; frame_num < frame_count; frame_num++)
  {
    char buf[4096];
    {
      uintptr_t rel_pc;
      char soname[4096];
      char best_name[4096];
      uintptr_t symbol_offset;
      char *p_soname = soname; //avoid -Werror=format-truncation
      char *p_best_name = best_name;
      if (backtrace_resolve(frames[frame_num], &rel_pc, soname, 4096, best_name, 4096, &symbol_offset))
      {
        snprintf(buf, sizeof(buf), "          #%02zd  pc %" PAD_PTR "  %s (%s+%" PRIuPTR ")\n", frame_num, rel_pc, p_soname, p_best_name, symbol_offset);
      }
      else
      {
        snprintf(buf, sizeof(buf), "          #%02zd  pc %" PAD_PTR "  %s\n", frame_num, rel_pc, p_soname);
      }
    }

    str += buf;
  }

  return str;
}

void backtrace_log(const uintptr_t *frames, size_t frame_count)
{
  fprintf(stderr, "%s", backtrace_string(frames, frame_count).c_str());
}
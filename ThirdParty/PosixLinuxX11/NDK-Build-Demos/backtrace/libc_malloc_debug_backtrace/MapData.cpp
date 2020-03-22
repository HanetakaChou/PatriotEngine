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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <ctype.h>

#include <elf.h>
#include <link.h>

#include <vector>

#include "MapData.h"

MapData::~MapData()
{
  entries_.clear();
}

// Find the containing map info for the PC.
MapEntry const *MapData::find(uintptr_t pc, uintptr_t *rel_pc)
{
  MapEntry pc_entry(pc);

  std::lock_guard<std::mutex> lock(m_);

  auto it = entries_.find(pc_entry);
  if (it == entries_.end())
  {
    SyncMaps();
  }

  it = entries_.find(pc_entry);
  if (it == entries_.end())
  {
    return NULL;
  }

  MapEntry const *entry = &(*it);

  if (rel_pc != NULL)
  {
    (*rel_pc) = entry->get_relpc(pc);
  }

  return entry;
}

bool MapData::SyncMaps()
{
  FILE *fp = fopen("/proc/self/maps", "re");
  if (fp == nullptr)
  {
    return false;
  }

  std::vector<char> buffer(1024);
  while (fgets(buffer.data(), buffer.size(), fp) != nullptr)
  {
    //try
    //{
    MapEntry entry = MapEntry::parse_line(buffer.data());
    //}
    //catch (...)
    //{
    //  fclose(fp);
    //  return false;
    //}

    auto it = entries_.find(entry);
    if (it == entries_.end())
    {
      entries_.insert(entry);
    }
  }
  fclose(fp);
  return true;
}

// Format of /proc/<PID>/maps:
//   6f000000-6f01e000 rwxp 00000000 00:0c 16389419   /system/lib/libcomposer.so
MapEntry MapEntry::parse_line(char *line)
{
  uintptr_t start;
  uintptr_t end;
  uintptr_t offset;
  char permissions[5];
  int name_pos;
  if (sscanf(line, "%" PRIxPTR "-%" PRIxPTR " %4s %" PRIxPTR " %*x:%*x %*d %n", &start, &end, permissions, &offset, &name_pos) < 2)
  {
    //throw;
    MapEntry entry(0, 0, 0, "<unknown>", strlen("<unknown>"));
    return entry;
  }

  const char *name = line + name_pos;
  size_t name_len = strlen(name);
  if (name_len && name[name_len - 1] == '\n')
  {
    name_len -= 1;
  }

  if (permissions[0] != 'r')
  {
    // Any unreadable map will just get a zero load base.
    MapEntry entry(start, end, offset, 0, name, name_len);
    return entry;
  }
  else
  {
    MapEntry entry(start, end, offset, name, name_len);
    return entry;
  }
}

template <typename T>
inline bool MapEntry::get_val(MapEntry const *entry, uintptr_t addr, T *store)
{
  if (addr < entry->start || addr + sizeof(T) > entry->end)
  {
    return false;
  }
  // Make sure the address is aligned properly.
  if (addr & (sizeof(T) - 1))
  {
    return false;
  }
  *store = *reinterpret_cast<T *>(addr);
  return true;
}

uintptr_t MapEntry::get_loadbase(MapEntry const *entry)
{
  uintptr_t addr = entry->start;

  ElfW(Ehdr) ehdr;
  if (!get_val<ElfW(Half)>(entry, addr + offsetof(ElfW(Ehdr), e_phnum), &ehdr.e_phnum))
  {
    return 0;
  }

  if (!get_val<ElfW(Off)>(entry, addr + offsetof(ElfW(Ehdr), e_phoff), &ehdr.e_phoff))
  {
    return 0;
  }

  addr += ehdr.e_phoff;
  for (size_t i = 0; i < ehdr.e_phnum; i++)
  {
    ElfW(Phdr) phdr;
    if (!get_val<ElfW(Word)>(entry, addr + offsetof(ElfW(Phdr), p_type),
                             &phdr.p_type))
    {
      return 0;
    }
    if (!get_val<ElfW(Off)>(entry, addr + offsetof(ElfW(Phdr), p_offset),
                            &phdr.p_offset))
    {
      return 0;
    }
    if (phdr.p_type == PT_LOAD && phdr.p_offset == entry->offset)
    {
      if (!get_val<ElfW(Addr)>(entry, addr + offsetof(ElfW(Phdr), p_vaddr),
                               &phdr.p_vaddr))
      {
        return 0;
      }
      return phdr.p_vaddr;
    }
    addr += sizeof(phdr);
  }

  return 0;
}

uintptr_t MapEntry::get_relpc(uintptr_t pc) const
{
  if (__builtin_expect(!m_load_base_init, false))
  {
    load_base = get_loadbase(this);
    m_load_base_init = true;
  }

  uintptr_t rel_pc = pc - start + load_base;
  return rel_pc;
}
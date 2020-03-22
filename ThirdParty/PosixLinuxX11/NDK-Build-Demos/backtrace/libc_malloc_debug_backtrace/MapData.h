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

#ifndef DEBUG_MALLOC_MAPDATA_H
#define DEBUG_MALLOC_MAPDATA_H

#include <sys/cdefs.h>

#include <mutex>
#include <set>
#include <string>

struct MapEntry
{
  static MapEntry parse_line(char *line);

  MapEntry(uintptr_t pc) : start(pc), end(pc)
  {
  }

  // Ordering comparator that returns equivalence for overlapping entries
  bool operator<(MapEntry const &other) const
  {
    MapEntry const &self = (*this);
    return self.end <= other.start;
  }

  char const *name() const
  {
    return m_name.c_str();
  }

  uintptr_t get_relpc(uintptr_t pc) const;

private:
  uintptr_t start;
  uintptr_t end;
  uintptr_t offset;
  uintptr_t load_base;
  std::string m_name;

  MapEntry(uintptr_t start, uintptr_t end, uintptr_t offset, const char *name, size_t name_len) : start(start), end(end), offset(offset), m_name(name, name_len)
  {

  }

  template <typename T>
  static inline bool get_val(MapEntry const *entry, uintptr_t addr, T *store);

  static uintptr_t get_loadbase(MapEntry const *entry);
};

class MapData
{
public:
  MapData() = default;
  ~MapData();

  const MapEntry *find(uintptr_t pc, uintptr_t *rel_pc = NULL);

private:
  bool SyncMaps();

  std::mutex m_;
  std::set<MapEntry> entries_;

  MapData(const MapData &) = delete;
  void operator=(const MapData &) = delete;
};

#endif // DEBUG_MALLOC_MAPDATA_H

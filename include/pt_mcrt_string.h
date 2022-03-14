//
// Copyright (C) YuqiaoZhang(HanetakaChou)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#ifndef _PT_MCRT_STRING_H_
#define _PT_MCRT_STRING_H_ 1

#include <string>

using mcrt_string = std::basic_string<char, std::char_traits<char>, mcrt::scalable_allocator<char>>;
using mcrt_wstring= std::basic_string<wchar_t, std::char_traits<wchar_t>, mcrt::scalable_allocator<wchar_t>>;

#endif
//
// Copyright (C) YuqiaoZhang(HanetakaYuminaga)
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

#include <stddef.h>
#include <pt_mcrt_log.h>
#include <stdarg.h>
#include <stdio.h>
#if defined(PT_POSIX)
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#elif defined(PT_WIN32)
    TODO
#else
#error Unknown Platform
#endif

int PT_CALL mcrt_log_write(char const *msg)
{

#if defined(PT_POSIX)
    return write(STDOUT_FILENO, msg, strlen(msg));
#elif defined(PT_WIN32)
    TODO
#else
#error Unknown Platform
#endif
}

#define LOG_BUFFER_SIZE 4096

int PT_CALL mcrt_log_print(char const *fmt, ...)
{
    char buf[LOG_BUFFER_SIZE];

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, LOG_BUFFER_SIZE, fmt, ap);
    va_end(ap);

    return mcrt_log_write(buf);
}


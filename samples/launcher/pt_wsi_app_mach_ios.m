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

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <Foundation/Foundation.h>

void get_main_bundle_resource_path(char *path, size_t *length)
{
    @autoreleasepool
    {
        char const *mainbundle_resource_path = [[[NSBundle mainBundle] resourcePath] UTF8String];
        size_t mainbundle_resource_path_length = strlen(mainbundle_resource_path);
        (*length) = mainbundle_resource_path_length;
        if (NULL != path)
        {
            memcpy(path, mainbundle_resource_path, sizeof(char) * mainbundle_resource_path_length);
        }
    }
}

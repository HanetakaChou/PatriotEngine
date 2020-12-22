/*
 * Copyright (C) YuqiaoZhang(HanetakaYuminaga)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <pt_common.h>
#include <pt_gfx_common.h>
#include "pt_gfx_malloc_common.h"
#include "pt_gfx_malloc_vk.h"

uint32_t gfx_malloc_vk::find_memory_type_index(uint32_t memory_requirements_memory_type_bits, VkMemoryPropertyFlags required_property_flags)
{
    uint32_t memory_type_count = m_physical_device_memory_properties.memoryTypeCount;
    for (uint32_t memory_type_index = 0; memory_type_index < memory_type_count; ++memory_type_index)
    {
        uint32_t memory_type_bits = (1U << memory_type_index);
        bool is_required_memory_type = ((memory_requirements_memory_type_bits & memory_type_bits) != 0) ? true : false;
        VkMemoryPropertyFlags property_flags = m_physical_device_memory_properties.memoryTypes[memory_type_index].propertyFlags;
        bool has_required_property_flags = ((property_flags & required_property_flags) != 0) ? true : false;

        if (is_required_memory_type && has_required_property_flags)
        {
            return memory_type_index;
        }
    }

    return VK_MAX_MEMORY_TYPES;
}

uint32_t gfx_malloc_vk::find_memory_type_index(uint32_t memory_requirements_memory_type_bits, VkMemoryPropertyFlags required_property_flags, VkMemoryPropertyFlags preferred_property_flags)
{
    VkMemoryPropertyFlags optimal_property_flags = (required_property_flags | preferred_property_flags);
    uint32_t memory_type_index = find_memory_type_index(memory_requirements_memory_type_bits, optimal_property_flags);
    if (VK_MAX_MEMORY_TYPES != memory_type_index)
    {
        return memory_type_index;
    }
    else
    {
        return find_memory_type_index(memory_requirements_memory_type_bits, required_property_flags);
    }
}

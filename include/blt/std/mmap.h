#pragma once
/*
 *  Copyright (C) 2024  Brett Terpstra
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef BLT_MMAP_H
#define BLT_MMAP_H

#include <blt/std/logging.h>
#include <blt/std/types.h>

#ifdef __unix__
    
    #include <sys/mman.h>

#endif

// size of 2mb in bytes
inline constexpr blt::size_t BLT_2MB_SIZE = 4096 * 512;

namespace blt
{
    
    /**
    * Logging function used for handling mmap errors. call after a failed mmap call.
    */
    void handle_mmap_error(blt::logging::logger func);
    
    inline static constexpr blt::size_t align_size_to(blt::size_t size, blt::size_t align)
    {
        const blt::size_t MASK = ~(align - 1);
        return (size & MASK) + align;
    }
    
    void* allocate_2mb_huge_pages(blt::size_t bytes);
    
}

#endif //BLT_MMAP_H

/*
 *  <Short Description>
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
#include <blt/math/fixed_point.h>
#include <blt/std/logging.h>
#include <iostream>
#include <blt_tests.h>

namespace blt::test
{
    void fixed_point()
    {
        fp64 uv = fp64::from_u64(32);
        fp64 iv = fp64::from_i64(16);
        
        std::cout << uv.as_i64() << " : " << uv.as_u64() << std::endl;
        std::cout << iv.as_i64() << " : " << iv.as_u64() << std::endl;
        
        std::cout << (uv * iv).as_i64() << std::endl;
        std::cout << (uv * iv).as_u64() << std::endl;
        std::cout << (uv / iv).as_i64() << std::endl;
        std::cout << (uv / iv).as_u64() << std::endl;
    }
}
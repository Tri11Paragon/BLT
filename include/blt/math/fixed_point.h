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

#ifndef BLT_FIXED_POINT_H
#define BLT_FIXED_POINT_H

#include <blt/std/types.h>
#include <blt/std/utility.h>

namespace blt
{
    struct fp64
    {
        private:
            u64 v = 0;
            
            fp64() = default;
            
            explicit fp64(u64 v): v(v)
            {}
        
        public:
            static fp64 from_u64(u64 ui)
            {
                fp64 fp;
                fp.v = ui << 32;
                return fp;
            }
            
            static fp64 from_i64(i64 si)
            {
                u64 ui = static_cast<u64>(si);
                fp64 fp;
                fp.v = ui << 32;
                return fp;
            }
            
            BLT_ATTRIB_NO_INLINE friend fp64 operator+(fp64 left, fp64 right)
            {
                return fp64(left.v + right.v);
            }
            
            BLT_ATTRIB_NO_INLINE friend fp64 operator-(fp64 left, fp64 right)
            {
                return fp64(left.v - right.v);
            }
            
            BLT_ATTRIB_NO_INLINE friend fp64 operator*(fp64 left, fp64 right)
            {
                auto lhs = static_cast<unsigned __int128>(left.v);
                auto rhs = static_cast<unsigned __int128>(right.v);
                return fp64(static_cast<u64>((lhs * rhs) >> 32));
            }
            
            BLT_ATTRIB_NO_INLINE friend fp64 operator/(fp64 left, fp64 right)
            {
                auto lhs = static_cast<unsigned __int128>(left.v);
                auto rhs = static_cast<unsigned __int128>(right.v);
                return fp64(static_cast<u64>((lhs / rhs) << 32));
            }
            
            [[nodiscard]] u64 as_u64() const
            {
                return v >> 32;
            }
            
            [[nodiscard]] i64 as_i64() const
            {
                return static_cast<i64>(v >> 32);
            }
    };
}

#endif //BLT_FIXED_POINT_H

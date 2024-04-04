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

#define BLT_DEBUG_NO_INLINE BLT_ATTRIB_NO_INLINE

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
            
            static fp64 from_f64(f64 d)
            {
                fp64 fp;
                fp.v = static_cast<u64>(d * (1ul << 32ul));
                return fp;
            }
            
            static fp64 from_f32(f32 f)
            {
                return from_f64(static_cast<double>(f));
            }
            
            BLT_DEBUG_NO_INLINE friend fp64 operator+(fp64 left, fp64 right)
            {
                return fp64(left.v + right.v);
            }
            
            BLT_DEBUG_NO_INLINE friend fp64 operator-(fp64 left, fp64 right)
            {
                return fp64(left.v - right.v);
            }
            
            BLT_DEBUG_NO_INLINE friend fp64 operator*(fp64 left, fp64 right)
            {
                auto lhs = static_cast<unsigned __int128>(left.v);
                auto rhs = static_cast<unsigned __int128>(right.v);
                return fp64(static_cast<u64>((lhs * rhs) >> 32));
            }
            
            BLT_DEBUG_NO_INLINE friend fp64 operator/(fp64 left, fp64 right)
            {
                auto lhs = static_cast<unsigned __int128>(left.v) << 32;
                auto rhs = static_cast<unsigned __int128>(right.v);
                
                return fp64(static_cast<u64>(lhs / rhs));
            }
            
            [[nodiscard]] inline u64 as_u64() const
            {
                return v >> 32;
            }
            
            [[nodiscard]] inline i64 as_i64() const
            {
                return static_cast<i64>(as_u64());
            }
            
            [[nodiscard]] inline u32 as_u32() const
            {
                return static_cast<u32>(as_u64());
            }
            
            [[nodiscard]] inline i32 as_i32() const
            {
                return static_cast<i32>(as_u64());
            }
            
            [[nodiscard]] inline f64 as_f64() const
            {
                return static_cast<f64>(v) / static_cast<f64>(1ul << 32ul);
            }
            
            [[nodiscard]] inline f32 as_f32() const
            {
                return static_cast<f32>(as_f64());
            }
            
            inline explicit operator u64() const
            {
                return as_u64();
            }
            
            inline explicit operator i64() const
            {
                return as_i64();
            }
            
            inline explicit operator u32() const
            {
                return as_u32();
            }
            
            inline explicit operator i32() const
            {
                return as_i32();
            }
            
            inline explicit operator f32() const
            {
                return as_f32();
            };
            
            inline explicit operator f64() const
            {
                return as_f64();
            }
            
            [[nodiscard]] u64 raw() const
            {
                return v;
            }
    };
}

#endif //BLT_FIXED_POINT_H

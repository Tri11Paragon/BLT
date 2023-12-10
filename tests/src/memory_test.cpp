/*
 *  Copyright (C) 2023  Brett Terpstra
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
#include <memory_test.h>

#include <blt/std/logging.h>
#include <blt/std/memory.h>
#include <blt/std/assert.h>
#include <blt/std/random.h>
#include <type_traits>

template<typename T>
blt::scoped_buffer<T> create_scoped_buffer(size_t size)
{
    static std::random_device dev;
    static std::mt19937_64 engine(dev());
    blt::scoped_buffer<T> data(size);
    if constexpr (std::is_floating_point_v<T>)
    {
        static std::uniform_real_distribution<T> dist(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
        
        for (auto& v : data)
            v = dist(engine);
    } else if (std::is_integral_v<T>)
    {
        static std::uniform_int_distribution<T> dist(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
        
        for (auto& v : data)
            v = dist(engine);
    }
    return data;
}

template<typename T>
blt::scoped_buffer<T> modify_copy(blt::scoped_buffer<T> fill)
{
    for (size_t i = 0; i < size_t(fill.size() / 2); i++)
    {
        std::swap(fill[i], fill[fill.size() - i - 1]);
    }
    return fill;
}

template<typename T>
T collect(blt::scoped_buffer<T> buff)
{
    T val = 0;
    for (auto v : buff)
        val = std::max(v, val);
    return val;
}

void blt::test::memory::copy()
{
    BLT_INFO("Running memory copy tests");
    
    auto int_buffer_small = create_scoped_buffer<int32_t>(16);
    auto int_buffer_medium = create_scoped_buffer<int32_t>(512);
    auto int_buffer_large = create_scoped_buffer<int32_t>(8192);
    
    auto float_buffer_small = create_scoped_buffer<float>(16);
    auto float_buffer_medium = create_scoped_buffer<float>(512);
    auto float_buffer_large = create_scoped_buffer<float>(8192);
    
    auto int_small = collect(modify_copy(int_buffer_small));
    auto int_medium = collect(modify_copy(int_buffer_medium));
    auto int_large = collect(modify_copy(int_buffer_large));
    
    auto float_small = collect(modify_copy(float_buffer_small));
    auto float_medium = collect(modify_copy(float_buffer_medium));
    auto float_large = collect(modify_copy(float_buffer_large));
    
    BLT_TRACE("We collected values [%d, %d, %d]; [%f.0, %f.0, %f.0]", int_small, int_medium, int_large, float_small, float_medium, float_large);
}

void blt::test::memory::move()
{
    BLT_INFO("Running memory move tests");
    
    
}

void blt::test::memory::access()
{
    BLT_INFO("Running memory construction tests");
    
}

void blt::test::memory::ranges()
{
    BLT_TRACE_STREAM << '\n';
}

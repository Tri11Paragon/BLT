/*
 *  <Short Description>
 *  Copyright (C) 2026  Brett Terpstra
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
#include <blt/iterator/iterator.h>

#include <blt/std/assert.h>
#include <blt/std/utility.h>
#include <blt/std/random.h>

template<typename T>
void fill(T& container)
{
    blt::random::random_t random{std::random_device{}()};
    for (size_t i = 0; i < random.get_u64(10, 30); i++)
        container.emplace(container.end(), random.get_u32(0, std::numeric_limits<blt::u32>::max()));
}

template<typename T, typename U>
void assert_type()
{
    BLT_ASSERT((std::is_same_v<T, U>));
    BLT_DEBUG("Types {} and {} are the same!", blt::type_string<T>(), blt::type_string<U>());
}

template<typename T>
void assert_const()
{
    using Type = std::remove_reference_t<T>;
    BLT_ASSERT((std::is_const_v<Type>));
    BLT_DEBUG("Type {} is const!", blt::type_string<T>());
}

template<typename T>
void assert_non_const()
{
    using Type = std::remove_reference_t<T>;
    BLT_ASSERT(!(std::is_const_v<Type>));
    BLT_DEBUG("Type {} is not const!", blt::type_string<T>());
}

template<typename Iter>
void const_return_type(Iter i)
{
    assert_const<blt::meta::deref_return_t<decltype(i.begin())>>();
}

template<typename Iter>
void non_const_return_type(Iter i)
{
    assert_non_const<blt::meta::deref_return_t<decltype(i.begin())>>();
}

template<typename Container>
auto iter(Container& container)
{
    return blt::iterate(container);
}

template<typename Container>
auto iter_const(const Container& container)
{
    return blt::iterate(container);
}

template<typename Container>
auto enumerate(Container& container)
{
    return blt::enumerate(container);
}

template<typename Container>
auto enumerate_const(const Container& container)
{
    return blt::enumerate(container);
}

int main()
{
    std::vector<blt::u32> container;
    fill(container);

    non_const_return_type(iter(container));
    const_return_type(iter_const(container));

    const_return_type(iter(container).as_const());
    const_return_type(iter_const(container).as_const());
}
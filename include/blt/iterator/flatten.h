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

#ifndef BLT_ITERATOR_FLATTEN_H
#define BLT_ITERATOR_FLATTEN_H

#include <blt/iterator/common.h>
#include <tuple>

namespace blt::iterator
{
    namespace detail
    {
        template<typename T>
        struct make_tuple
        {
            using type = std::tuple<T>;

            static auto convert(T& f)
            {
                return std::forward_as_tuple(f);
            }

            static auto convert(T&& f)
            {
                return std::forward_as_tuple(f);
            }
        };

        template<typename... Args>
        struct make_tuple<std::tuple<Args...>>
        {
            using type = std::tuple<Args...>;

            static std::tuple<Args...>& convert(std::tuple<Args...>& lvalue)
            {
                return lvalue;
            }

            static std::tuple<Args...>&& convert(std::tuple<Args...>&& rvalue)
            {
                return rvalue;
            }
        };
    }

    template <typename Iter>
    class flatten_wrapper : public deref_only_wrapper<Iter, flatten_wrapper<Iter>>
    {
    public:
        using iterator_category = typename std::iterator_traits<Iter>::iterator_category;
        // using value_type = std::invoke_result_t<Func, meta::deref_return_t<Iter>>;
        using difference_type = ptrdiff_t;
        // using pointer = value_type;
        // using reference = value_type;

        explicit flatten_wrapper(Iter iter):
            deref_only_wrapper<Iter, flatten_wrapper<Iter>>(std::move(iter))
        {
        }

        reference operator*() const
        {
            return func(*this->iter);
        }
    };
}


#endif //BLT_ITERATOR_FLATTEN_H

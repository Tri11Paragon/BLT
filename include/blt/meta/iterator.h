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

#ifndef BLT_META_ITERATOR_H
#define BLT_META_ITERATOR_H

#include <type_traits>
#include <iterator>

namespace blt::meta
{
    
    template<typename Iter>
    constexpr bool is_input_iterator = std::is_same_v<typename std::iterator_traits<Iter>::iterator_cateogry, std::input_iterator_tag>;
    
    template<typename Iter>
    constexpr bool is_forward_iterator = std::is_same_v<typename std::iterator_traits<Iter>::iterator_cateogry, std::forward_iterator_tag>;
    
    template<typename Iter>
    constexpr bool is_bidirectional_iterator = std::is_same_v<typename std::iterator_traits<Iter>::iterator_cateogry, std::bidirectional_iterator_tag>;
    
    template<typename Iter>
    constexpr bool is_random_access_iterator = std::is_same_v<typename std::iterator_traits<Iter>::iterator_cateogry, std::random_access_iterator_tag>;
    
    template<typename Iter>
    constexpr bool is_bidirectional_or_better = is_bidirectional_iterator<Iter> || is_random_access_iterator<Iter>;
}

#endif //BLT_META_ITERATOR_H

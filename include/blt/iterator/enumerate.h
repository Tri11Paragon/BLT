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

#ifndef BLT_ITERATOR_ENUMERATE_H
#define BLT_ITERATOR_ENUMERATE_H

#include <blt/iterator/common.h>
#include <blt/meta/meta.h>
#include <tuple>

namespace blt
{
    
    namespace iterator
    {
        /**
         * struct which is returned by the enumerator.
         * @tparam T type to store.
         */
        template<typename T>
        struct enumerate_item
        {
            blt::size_t index;
            T value;
        };
        
        template<typename Iter>
        class enumerate_wrapper : public passthrough_wrapper<Iter, enumerate_wrapper<Iter>>
        {
            public:
                using passthrough_wrapper<Iter, enumerate_wrapper<Iter>>::passthrough_wrapper;
                
                enumerate_item<meta::deref_return_t<Iter>> operator*() const
                {
                    return *this->iter;
                }
            private:
                blt::size_t index;
        };
    }
    
}

#endif //BLT_ITERATOR_ENUMERATE_H

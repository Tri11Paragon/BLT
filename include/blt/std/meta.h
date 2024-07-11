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

#ifndef BLT_META_H
#define BLT_META_H

#include <blt/std/types.h>
#include <utility>

namespace blt::meta
{
    template<typename IFirst, typename... IArgs>
    struct arg_helper
    {
        using First = IFirst;
        using Next = arg_helper<IArgs...>;
    };
    
    template<>
    struct arg_helper<void>
    {
        using First = void;
        using Next = void;
    };
    
    template<typename TheLambda, typename>
    struct lambda_helper
    {
        using Lambda = TheLambda;
    };
    
    template<typename TheLambda, typename IReturn, typename IClass, typename... LArgs>
    struct lambda_helper<TheLambda, IReturn(IClass::*)(LArgs...)>
    {
        using Lambda = TheLambda;
        using Return = IReturn;
        using Class = IClass;
        using Args = arg_helper<LArgs...>;
    };
    
    template<typename Lambda>
    lambda_helper(Lambda) -> lambda_helper<Lambda, decltype(&Lambda::operator())>;
    
    
}

#endif //BLT_GP_META_H

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
#include <type_traits>
#include <ostream>

namespace blt::meta
{
    template<typename IFirst = std::void_t<void>, typename... IArgs>
    struct arg_helper
    {
        using First = IFirst;
        using Next = arg_helper<IArgs...>;
    };
    
    template<typename IFirst>
    struct arg_helper<IFirst>
    {
        using First = IFirst;
        using Next = void;
    };
    
    template<>
    struct arg_helper<>
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
    struct lambda_helper<TheLambda, IReturn (IClass::*)(LArgs...) const>
    {
        using Lambda = TheLambda;
        using Return = IReturn;
        using Class = IClass;
        using Args = arg_helper<LArgs...>;
        
        template<typename T>
        explicit lambda_helper(T)
        {}
        
        lambda_helper() = default;
    };
    
    template<typename Lambda>
    lambda_helper(Lambda) -> lambda_helper<Lambda, decltype(&Lambda::operator())>;
    
    // https://stackoverflow.com/questions/66397071/is-it-possible-to-check-if-overloaded-operator-for-type-or-class-exists
    template<typename T>
    class is_streamable
    {
        private:
            template<typename Subs>
            static auto test(int) -> decltype(std::declval<std::ostream&>() << std::declval<Subs>(), std::true_type())
            {
                return std::declval<std::true_type>();
            }
            
            template<typename>
            static auto test(...) -> std::false_type
            {
                return std::declval<std::false_type>();
            }
        
        public:
            static constexpr bool value = decltype(test<T>(0))::value;
    };
    
    template<class T>
    inline constexpr bool is_streamable_v = is_streamable<T>::value;
    
    namespace detail
    {
        template<typename Or>
        struct value_type_helper
        {
            template<typename Subs>
            inline static constexpr auto get(int) -> typename Subs::value_type
            {
                return std::declval<Subs::value_type>();
            }
            
            template<typename>
            inline static constexpr Or get(...)
            {
                return std::declval<Or>();
            }
        };
        
        template<typename Or>
        struct reference_type_helper
        {
            template<typename Subs>
            inline static constexpr auto get(int) -> typename Subs::reference
            {
                return std::declval<typename Subs::reference>();
            }
            
            template<typename>
            inline static constexpr Or get(...)
            {
                return std::declval<Or>();
            }
        };
        
        template<typename Or>
        struct const_reference_type_helper
        {
            template<typename Subs>
            inline static constexpr auto get(int) -> typename Subs::const_reference
            {
                return std::declval<typename Subs::const_reference>();
            }
            
            template<typename>
            inline static constexpr Or get(...)
            {
                return std::declval<Or>();
            }
        };
        
        template<typename Or>
        struct pointer_type_helper
        {
            template<typename Subs>
            inline static constexpr auto get(int) -> typename Subs::pointer
            {
                return std::declval<typename Subs::pointer>();
            }
            
            template<typename>
            inline static constexpr Or get(...)
            {
                return std::declval<Or>();
            }
        };
        
        template<typename Or>
        struct difference_type_helper
        {
            template<typename Subs>
            inline static constexpr auto get(int) -> typename Subs::difference_type
            {
                return std::declval<typename Subs::difference_type>();
            }
            
            template<typename>
            inline static constexpr Or get(...)
            {
                return std::declval<Or>();
            }
        };
    }
    
    template<typename T, typename Or>
    using value_type_t = decltype(detail::value_type_helper<Or>::template get<T>(0));
    template<typename T, typename Or>
    using difference_t = decltype(detail::difference_type_helper<Or>::template get<T>(0));
    template<typename T, typename Or>
    using pointer_t = decltype(detail::pointer_type_helper<Or>::template get<T>(0));
    template<typename T, typename Or>
    using reference_t = decltype(detail::reference_type_helper<Or>::template get<T>(0));
    template<typename T, typename Or>
    using const_reference_t = decltype(detail::const_reference_type_helper<Or>::template get<T>(0));

#define BLT_META_MAKE_FUNCTION_CHECK(FUNC, ...)\
    template<typename T, typename = void> \
    class has_func_##FUNC : public std::false_type \
    {}; \
    template<typename T> \
    class has_func_##FUNC<T, std::void_t<decltype(std::declval<T>().FUNC(,##__VA_ARGS__))>> : public std::true_type \
    {}; \
    template<typename T> \
    inline constexpr bool has_func_##FUNC##_v = has_func_##FUNC<T>::value;


#define BLT_META_MAKE_MEMBER_CHECK(MEMBER)\
    template<typename T, typename = void> \
    class has_member_##MEMBER : public std::false_type \
    {}; \
    template<typename T> \
    class has_member_##MEMBER<T, std::void_t<decltype(T::MEMBER)>> : public std::true_type \
    {}; \
    template<typename T> \
    inline constexpr bool has_member_##MEMBER##_v = has_member_##MEMBER<T>::value;
    
    
}

#endif //BLT_GP_META_H

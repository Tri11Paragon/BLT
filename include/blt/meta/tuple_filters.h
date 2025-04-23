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

#ifndef BLT_META_FILTER_VOID_H
#define BLT_META_FILTER_VOID_H

#include <tuple>
#include <type_traits>

namespace blt::meta
{
	template <typename... Ts>
	struct filter_void;

	template <>
	struct filter_void<>
	{
		using type = std::tuple<>;
	};

	template <typename T, typename... Ts>
	struct filter_void<T, Ts...>
	{
		using type = std::conditional_t<std::is_void_v<T>, typename filter_void<Ts...>::type, decltype(std::tuple_cat(
											std::declval<std::tuple<T>>(), std::declval<typename filter_void<Ts...>::type>()))>;
	};

	template <typename... Ts>
	struct filter_void<std::tuple<Ts...>>
	{
		using type = typename filter_void<Ts...>::type;
	};

	template <typename... Ts>
	using filter_void_t = typename filter_void<Ts...>::type;

	template <template<typename...> typename Func, typename ArgsTuple, typename... Ts>
	struct filter_func;

	template <template<typename...> typename Func, typename ArgsTuple>
	struct filter_func<Func, ArgsTuple>
	{
		using type = std::tuple<>;
	};

	template <template<typename...> typename Func, typename... Args, typename T, typename... Ts>
	struct filter_func<Func, std::tuple<Args...>, T, Ts...>
	{
		using type = std::conditional_t<Func<T, Args...>::value, typename filter_func<Func, std::tuple<Args...>, Ts...>::type, decltype(
											std::tuple_cat(std::declval<std::tuple<T>>(),
															std::declval<typename filter_func<Func, std::tuple<Args...>, Ts...>::type>()))>;
	};

	template <template<typename...> typename Func, typename ArgsTuple, typename... Ts>
	struct filter_func<Func, ArgsTuple, std::tuple<Ts...>>
	{
		using type = typename filter_func<Func, ArgsTuple, Ts...>::type;
	};

	template <template<typename...> typename Func, typename ArgsTuple, typename... Ts>
	using filter_func_t = typename filter_func<Func, ArgsTuple, Ts...>::type;

}

#endif //BLT_META_FILTER_VOID_H

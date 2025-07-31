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

#ifndef BLT_META_IS_STREAMABLE_H
#define BLT_META_IS_STREAMABLE_H

#include <ostream>

namespace blt::meta
{
	/*
	 * For future me:
	 * The reason the functions take in an int value is because it forces the C++ to look at overloads when calling the function.
	 * If you don't take in a value, you will get an ambitious function error.
	 * This way if it fails to substitute into the int (your test) then it can into the generic parameter. It will choose
	 * the int function first always because it matches the type of your input value (0)
	 *
	 * TODO: you can probably replace most of this with std::void_t?
	 *
	 */

	// https://stackoverflow.com/questions/66397071/is-it-possible-to-check-if-overloaded-operator-for-type-or-class-exists

	/**
	 * Helper class to determine if the input type can be output to a std::ostream (default)
	 */
	template <typename T, typename Stream = std::ostream>
	class is_streamable
	{
		template <typename Subs>
		static auto test(int) -> decltype(std::declval<Stream&>() << std::declval<Subs>(), std::true_type())
		{
			return std::declval<std::true_type>();
		}

		template <typename>
		static auto test(...) -> std::false_type { return std::declval<std::false_type>(); }

	public:
		static constexpr bool value = decltype(test<T>(0))::value;
	};


	template <class T, typename Stream = std::ostream>
	inline constexpr bool is_streamable_v = is_streamable<T, Stream>::value;

	/**
	 * Helper class to determine if the input type is a container (has a .data and a .size function)
	 */
	template <typename T>
	class is_container
	{
		template <typename Subs>
		static auto test(int) -> decltype(std::declval<Subs>().data(), std::declval<Subs>().size(), std::true_type())
		{
			return std::declval<std::true_type>();
		}

		template <typename>
		static auto test(...) -> std::false_type { return std::declval<std::false_type>(); }

	public:
		static constexpr bool value = decltype(test<T>(0))::value;
	};


	template <class T>
	inline constexpr bool is_container_v = is_container<T>::value;


	template <typename T>
	class is_iterable
	{
		template <typename Subs>
		static auto test(int) -> decltype(std::declval<Subs>().begin(), std::declval<Subs>().end(), std::true_type())
		{
			return std::declval<std::true_type>();
		}

		template <typename>
		static auto test(...) -> std::false_type { return std::declval<std::false_type>(); }

	public:
		static constexpr bool value = decltype(test<T>(0))::value;
	};


	template <class T>
	inline constexpr bool is_iterable_v = is_iterable<T>::value;


	template <typename Container, typename... Args>
	class has_insert
	{
		template <typename Subs>
		static auto test(int) -> decltype(std::declval<Subs>().insert(std::declval<Args>()...), std::true_type())
		{
			return std::declval<std::true_type>();
		}

		template <typename>
		static auto test(...) -> std::false_type { return std::declval<std::false_type>(); }

	public:
		static constexpr bool value = decltype(test<Container>(0))::value;
	};


	template <class Container, typename... Args>
	inline constexpr bool has_insert_v = has_insert<Container, Args...>::value;


	template <typename Container, typename... Args>
	class has_iter_insert
	{
		template <typename Subs>
		static auto test(
			int) -> decltype(std::declval<Subs>().insert(std::declval<Subs>().end(), std::declval<Args>()...),
							 std::true_type()) { return std::declval<std::true_type>(); }

		template <typename>
		static auto test(...) -> std::false_type { return std::declval<std::false_type>(); }

	public:
		static constexpr bool value = decltype(test<Container>(0))::value;
	};


	template <class Container, typename... Args>
	inline constexpr bool has_iter_insert_v = has_iter_insert<Container, Args...>::value;


	template <typename Container, typename... Args>
	class has_push_back
	{
		template <typename Subs>
		static auto test(int) -> decltype(std::declval<Subs>().push_back(std::declval<Args>()...), std::true_type())
		{
			return std::declval<std::true_type>();
		}

		template <typename>
		static auto test(...) -> std::false_type { return std::declval<std::false_type>(); }

	public:
		static constexpr bool value = decltype(test<Container>(0))::value;
	};


	template <class Container, typename... Args>
	inline constexpr bool has_push_back_v = has_push_back<Container, Args...>::value;


	template <typename Container, typename... Args>
	class has_push_front
	{
		template <typename Subs>
		static auto test(int) -> decltype(std::declval<Subs>().push_front(std::declval<Args>()...), std::true_type())
		{
			return std::declval<std::true_type>();
		}

		template <typename>
		static auto test(...) -> std::false_type { return std::declval<std::false_type>(); }

	public:
		static constexpr bool value = decltype(test<Container>(0))::value;
	};


	template <class Container, typename... Args>
	inline constexpr bool has_push_front_v = has_push_front<Container, Args...>::value;


	template <typename Container, typename... Args>
	class has_push
	{
		template <typename Subs>
		static auto test(int) -> decltype(std::declval<Subs>().push(std::declval<Args>()...), std::true_type())
		{
			return std::declval<std::true_type>();
		}

		template <typename>
		static auto test(...) -> std::false_type { return std::declval<std::false_type>(); }

	public:
		static constexpr bool value = decltype(test<Container>(0))::value;
	};


	template <class Container, typename... Args>
	inline constexpr bool has_push_v = has_push<Container, Args...>::value;

	namespace detail
	{
		/**
		 * Checks if the given container has a push function which accepts the element.
		 * We should prefer using a push function over the insert functions.
		 * There is no concrete reason for this beyond a perceived and likely misguided belief that push functions
		 * *might* be faster to insert with.
		 */
		template <typename Container, typename Element>
		inline constexpr bool has_push_func = has_push_back_v<Container, Element> || has_push_front_v<
												  Container, Element> || has_push_v<Container, Element>;

		// this one is clear, insertion to the front is often an O(n) operation. If a container supports both, prefer push_back
		template <typename Container, typename Element>
		inline constexpr bool front_prefer_push_back =
			has_push_front_v<Container, Element> && !has_push_back_v<Container, Element>;

		// in the same vein as `front_prefer_push_back` except this is unlikely to be an issue.
		template <typename Container, typename Element>
		inline constexpr bool push_prefer_push_back =
			has_push_v<Container, Element> && !has_push_back_v<Container, Element>;

		template <typename Container, typename Element>
		inline constexpr bool enable_insert_iter =
			has_iter_insert_v<Container, Element> && !has_insert_v<Container, Element> && !has_push_func<
				Container, Element>;

		// given the nature of classes which take single elements to their insert functions, it is unlikely the push check is needed.
		template <typename Container, typename Element>
		inline constexpr bool enable_insert = has_insert_v<Container, Element> && !has_push_func<Container, Element>;
	}

	/*
	 * Generic insert helper which should work on most containers. Attempts to insert the provided element
	 */
	template <typename Container>
	struct insert_helper_t
	{
		template <typename G, std::enable_if_t<detail::enable_insert_iter<Container, G>, bool>  = true>
		insert_helper_t(Container& container, G&& element)
		{
			container.insert(container.end(), std::forward<G>(element));
		}

		template <typename G, std::enable_if_t<detail::enable_insert<Container, G>, bool>  = true>
		insert_helper_t(Container& container, G&& element) { container.insert(std::forward<G>(element)); }

		template <typename G, std::enable_if_t<has_push_back_v<Container, G>, bool>  = true>
		insert_helper_t(Container& container, G&& element) { container.push_back(std::forward<G>(element)); }

		template <typename G, std::enable_if_t<detail::front_prefer_push_back<Container, G>, bool>  = true>
		insert_helper_t(Container& container, G&& element) { container.push_front(std::forward<G>(element)); }

		template <typename G, std::enable_if_t<detail::push_prefer_push_back<Container, G>, bool>  = true>
		insert_helper_t(Container& container, G&& element) { container.push(std::forward<G>(element)); }
	};


	template <typename Container>
	insert_helper_t(Container) -> insert_helper_t<Container>;
}

#endif //BLT_META_IS_STREAMABLE_H

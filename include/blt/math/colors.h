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

#ifndef BLT_MATH_COLORS_H
#define BLT_MATH_COLORS_H

#include <blt/math/vectors.h>
#include <blt/std/types.h>
#include <blt/std/variant.h>

namespace blt
{
	class color_t
	{
	public:
		struct color_linear_rgb_t : integer_type<vec3>
		{};


		struct color_srgb_t : integer_type<vec3>
		{};


		struct color_oklab_t : integer_type<vec3>
		{};


		struct color_hsv_t : integer_type<vec3>
		{};


		using color_variant_t = std::variant<color_linear_rgb_t, color_srgb_t, color_oklab_t, color_hsv_t>;

		color_t() = default;

		template <typename Type>
		explicit color_t(const vec3& color) : color{Type{color}}
		{}

		explicit color_t(const color_variant_t color) : color{color}
		{}

		[[nodiscard]] vec3 as_linear_rgb() const
		{
			return variant_t{color}
			.visit([](const color_linear_rgb_t c) {
					   return static_cast<vec3>(c);
				   },
				   [](const color_srgb_t c) {
					   return static_cast<vec3>(c).srgb_to_linear_rgb();
				   },
				   [](const color_oklab_t c) {
					   return static_cast<vec3>(c).oklab_to_linear_rgb();
				   },
				   [](const color_hsv_t c) {
					   return static_cast<vec3>(c).hsv_to_linear_rgb();
				   }).value();
		}

		[[nodiscard]] vec3 as_srgb() const
		{
			return variant_t{color}
			.visit([](const color_linear_rgb_t c) {
					   return static_cast<vec3>(c).linear_to_srgb();
				   },
				   [](const color_srgb_t c) {
					   return static_cast<vec3>(c);
				   },
				   [](const color_oklab_t c) {
					   return static_cast<vec3>(c).oklab_to_linear_rgb().linear_to_srgb();
				   },
				   [](const color_hsv_t c) {
					   return static_cast<vec3>(c).hsv_to_linear_rgb().linear_to_srgb();
				   }).value();
		}

		[[nodiscard]] vec3 as_oklab() const
		{
			return variant_t{color}
			.visit([](const color_linear_rgb_t c) {
					   return static_cast<vec3>(c).linear_rgb_to_oklab();
				   },
				   [](const color_srgb_t c) {
					   return static_cast<vec3>(c).srgb_to_linear_rgb().
														linear_rgb_to_oklab();
				   },
				   [](const color_oklab_t c) {
					   return static_cast<vec3>(c);
				   },
				   [](const color_hsv_t c) {
					   return static_cast<vec3>(c).hsv_to_linear_rgb().
														linear_rgb_to_oklab();
				   }).value();
		}

		[[nodiscard]] vec3 as_hsv() const
		{
			return variant_t{color}
			.visit([](const color_linear_rgb_t c) {
					   return static_cast<vec3>(c).linear_rgb_to_hsv();
				   },
				   [](const color_srgb_t c) {
					   return static_cast<vec3>(c).srgb_to_linear_rgb().
														linear_rgb_to_hsv();
				   },
				   [](const color_oklab_t c) {
					   return static_cast<vec3>(c).oklab_to_linear_rgb().
														linear_rgb_to_hsv();
				   },
				   [](const color_hsv_t c) {
					   return static_cast<vec3>(c);
				   }).value();
		}

	private:
		color_variant_t color;
	};
}

#endif //BLT_MATH_COLORS_H

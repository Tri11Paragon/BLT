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
	namespace color
	{
		struct color_linear_rgb_t;
		struct color_srgb_t;
		struct color_oklab_t;
		struct color_oklch_t;
		struct color_hsv_t;

		struct color_linear_rgb_t : integer_type<vec3>
		{
			using integer_type::integer_type;

			[[nodiscard]] color_linear_rgb_t to_linear_rgb() const;
			[[nodiscard]] color_srgb_t       to_srgb() const;
			[[nodiscard]] color_oklab_t      to_oklab() const;
			[[nodiscard]] color_oklch_t      to_oklch() const;
			[[nodiscard]] color_hsv_t        to_hsv() const;
		};


		struct color_srgb_t : integer_type<vec3>
		{
			using integer_type::integer_type;

			[[nodiscard]] color_linear_rgb_t to_linear_rgb() const;
			[[nodiscard]] color_srgb_t       to_srgb() const;
			[[nodiscard]] color_oklab_t      to_oklab() const;
			[[nodiscard]] color_oklch_t      to_oklch() const;
			[[nodiscard]] color_hsv_t        to_hsv() const;
		};


		struct color_oklab_t : integer_type<vec3>
		{
			using integer_type::integer_type;

			[[nodiscard]] color_linear_rgb_t to_linear_rgb() const;
			[[nodiscard]] color_srgb_t       to_srgb() const;
			[[nodiscard]] color_oklab_t      to_oklab() const;
			[[nodiscard]] color_oklch_t      to_oklch() const;
			[[nodiscard]] color_hsv_t        to_hsv() const;
		};


		struct color_oklch_t : integer_type<vec3>
		{
			using integer_type::integer_type;

			[[nodiscard]] color_linear_rgb_t to_linear_rgb() const;
			[[nodiscard]] color_srgb_t       to_srgb() const;
			[[nodiscard]] color_oklab_t      to_oklab() const;
			[[nodiscard]] color_oklch_t      to_oklch() const;
			[[nodiscard]] color_hsv_t        to_hsv() const;
		};


		struct color_hsv_t : integer_type<vec3>
		{
			using integer_type::integer_type;

			[[nodiscard]] color_linear_rgb_t to_linear_rgb() const;
			[[nodiscard]] color_srgb_t       to_srgb() const;
			[[nodiscard]] color_oklab_t      to_oklab() const;
			[[nodiscard]] color_oklch_t      to_oklch() const;
			[[nodiscard]] color_hsv_t        to_hsv() const;
		};
	}

	class color_t
	{
	public:
		using color_linear_rgb_t = color::color_linear_rgb_t;
		using color_srgb_t       = color::color_srgb_t;
		using color_oklab_t      = color::color_oklab_t;
		using color_oklch_t      = color::color_oklch_t;
		using color_hsv_t        = color::color_hsv_t;

		using color_variant_t = std::variant<color_linear_rgb_t, color_srgb_t, color_oklab_t, color_oklch_t,
											 color_hsv_t>;

		color_t() = default;

		template <typename Type>
		static color_t from(const vec3& color)
		{
			return color_t{Type{color}};
		}

		explicit color_t(const color_linear_rgb_t& color) : color{color}
		{}

		explicit color_t(const color_srgb_t& color) : color{color}
		{}

		explicit color_t(const color_oklab_t& color) : color{color}
		{}

		explicit color_t(const color_oklch_t& color) : color{color}
		{}

		explicit color_t(const color_hsv_t& color) : color{color}
		{}

		[[nodiscard]] color_linear_rgb_t as_linear_rgb() const
		{
			return std::visit([](const auto& c) {
				return c.to_linear_rgb();
			}, color);
		}

		[[nodiscard]] color_srgb_t as_srgb() const
		{
			return std::visit([](const auto& c) {
				return c.to_srgb();
			}, color);
		}

		[[nodiscard]] color_oklab_t as_oklab() const
		{
			return std::visit([](const auto& c) {
				return c.to_oklab();
			}, color);
		}

		[[nodiscard]] color_oklch_t as_oklch() const
		{
			return std::visit([](const auto& c) {
				return c.to_oklch();
			}, color);
		}

		[[nodiscard]] color_hsv_t as_hsv() const
		{
			return std::visit([](const auto& c) {
				return c.to_hsv();
			}, color);
		}

	private:
		color_variant_t color;
	};
}

#endif //BLT_MATH_COLORS_H

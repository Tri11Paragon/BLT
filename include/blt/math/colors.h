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
	template <typename T>
	T srgb_to_linear(const T c) noexcept
	{
		return (c <= 0.04045) ? c / 12.92 : std::pow((c + 0.055) / 1.055, 2.4);
	}

	template <typename T>
	T linear_to_srgb(const T c) noexcept
	{
		return (c <= 0.0031308) ? 12.92 * c : 1.055 * std::pow(c, 1.0 / 2.4) - 0.055;
	}

	class color_t;

	namespace color
	{
		struct linear_rgb_t;
		struct srgb_t;
		struct oklab_t;
		struct oklch_t;
		struct hsv_t;

		struct color_base : integer_type<vec3>
		{
			using integer_type::integer_type;

			[[nodiscard]] vec3 to_vec3() const
			{
				return static_cast<vec3>(*this);
			}

			[[nodiscard]] std::array<float, 3> unpack() const
			{
				const auto v = to_vec3();
				return {v.x(), v.y(), v.z()};
			}
		};

		struct linear_rgb_t : color_base
		{
			using color_base::color_base;

			[[nodiscard]] linear_rgb_t to_linear_rgb() const;
			[[nodiscard]] srgb_t       to_srgb() const;
			[[nodiscard]] oklab_t      to_oklab() const;
			[[nodiscard]] oklch_t      to_oklch() const;
			[[nodiscard]] hsv_t        to_hsv() const;
		};


		struct srgb_t : color_base
		{
			using color_base::color_base;

			[[nodiscard]] linear_rgb_t to_linear_rgb() const;
			[[nodiscard]] srgb_t       to_srgb() const;
			[[nodiscard]] oklab_t      to_oklab() const;
			[[nodiscard]] oklch_t      to_oklch() const;
			[[nodiscard]] hsv_t        to_hsv() const;
		};


		struct oklab_t : color_base
		{
			using color_base::color_base;

			[[nodiscard]] linear_rgb_t to_linear_rgb() const;
			[[nodiscard]] srgb_t       to_srgb() const;
			[[nodiscard]] oklab_t      to_oklab() const;
			[[nodiscard]] oklch_t      to_oklch() const;
			[[nodiscard]] hsv_t        to_hsv() const;
		};


		struct oklch_t : color_base
		{
			using color_base::color_base;

			[[nodiscard]] linear_rgb_t to_linear_rgb() const;
			[[nodiscard]] srgb_t       to_srgb() const;
			[[nodiscard]] oklab_t      to_oklab() const;
			[[nodiscard]] oklch_t      to_oklch() const;
			[[nodiscard]] hsv_t        to_hsv() const;
		};


		struct hsv_t : color_base
		{
			using color_base::color_base;

			[[nodiscard]] linear_rgb_t to_linear_rgb() const;
			[[nodiscard]] srgb_t       to_srgb() const;
			[[nodiscard]] oklab_t      to_oklab() const;
			[[nodiscard]] oklch_t      to_oklch() const;
			[[nodiscard]] hsv_t        to_hsv() const;
		};


		template <typename Type>
		static color_t from(const vec3& color);

		template <typename Type>
		static color_t from(const vec4& color);
	}


	class color_t
	{
	public:
		using color_linear_rgb_t = color::linear_rgb_t;
		using color_srgb_t       = color::srgb_t;
		using color_oklab_t      = color::oklab_t;
		using color_oklch_t      = color::oklch_t;
		using color_hsv_t        = color::hsv_t;

		using color_variant_t = std::variant<color_linear_rgb_t, color_srgb_t, color_oklab_t, color_oklch_t,
											 color_hsv_t>;

		color_t() = default;

		color_t(const color_linear_rgb_t& color) : color{color} // NOLINT
		{}

		color_t(const color_srgb_t& color) : color{color} // NOLINT
		{}

		color_t(const color_oklab_t& color) : color{color} // NOLINT
		{}

		color_t(const color_oklch_t& color) : color{color} // NOLINT
		{}

		color_t(const color_hsv_t& color) : color{color} // NOLINT
		{}

		[[nodiscard]] color_linear_rgb_t as_linear_rgb() const
		{
			return std::visit([](const auto& c) {
								  return c.to_linear_rgb();
							  },
							  color);
		}

		[[nodiscard]] color_srgb_t as_srgb() const
		{
			return std::visit([](const auto& c) {
								  return c.to_srgb();
							  },
							  color);
		}

		[[nodiscard]] color_oklab_t as_oklab() const
		{
			return std::visit([](const auto& c) {
								  return c.to_oklab();
							  },
							  color);
		}

		[[nodiscard]] color_oklch_t as_oklch() const
		{
			return std::visit([](const auto& c) {
								  return c.to_oklch();
							  },
							  color);
		}

		[[nodiscard]] color_hsv_t as_hsv() const
		{
			return std::visit([](const auto& c) {
								  return c.to_hsv();
							  },
							  color);
		}

		/**
		 * WARNING! this function is unsafe as it ignores internal validation of stored colors, you should only use it if you know what you are doing. !
		 * This function is provided for convenience when you already know what types are held in the color.
		 */
		[[nodiscard]] vec3 to_vec3() const
		{
			return std::visit([](const auto& c) {
								  return c.to_vec3();
							  },
							  color);
		}

		/**
		 * WARNING! This function is unsafe as it bypasses internal validation of stored color. It is up to you to use
		 * the information in the variant correctly, if not using the class functions.
		 * This function is provided for extensibility to developers.
		 */
		[[nodiscard]] const color_variant_t& variant() const
		{
			return color;
		}

	private:
		color_variant_t color;
	};


	template <typename Type>
	static color_t color::from(const vec3& color)
	{
		return color_t{Type{color}};
	}

	template <typename Type>
	static color_t color::from(const vec4& color)
	{
		return color_t{Type{make_vec3(color) * color.a()}};
	}
}

#endif //BLT_MATH_COLORS_H

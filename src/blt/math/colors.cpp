/*
 *  <Short Description>
 *  Copyright (C) 2025  Brett Terpstra
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
#include <blt/math/colors.h>

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

	color::linear_rgb_t color::linear_rgb_t::to_linear_rgb() const
	{
		return *this;
	}

	color::srgb_t color::linear_rgb_t::to_srgb() const
	{
		auto copy = static_cast<vec3>(*this);
		copy[0]   = linear_to_srgb(copy[0]);
		copy[1]   = linear_to_srgb(copy[1]);
		copy[2]   = linear_to_srgb(copy[2]);
		return copy;
	}

	color::oklab_t color::linear_rgb_t::to_oklab() const
	{
		const auto   self = static_cast<vec3>(*this);
		const double l    = 0.4122214708 * self.r() + 0.5363325363 * self.g() + 0.0514459929 * self.b();
		const double m    = 0.2119034982 * self.r() + 0.6806995451 * self.g() + 0.1073969566 * self.b();
		const double s    = 0.0883024619 * self.r() + 0.2817188376 * self.g() + 0.6299787005 * self.b();

		// 3. cube root
		const double l_ = cbrt(l);
		const double m_ = cbrt(m);
		const double s_ = cbrt(s);

		// 4. LMS′ → OKLab
		vec3 out;
		out[0] = 0.2104542553 * l_ + 0.7936177850 * m_ - 0.0040720468 * s_;
		out[1] = 1.9779984951 * l_ - 2.4285922050 * m_ + 0.4505937099 * s_;
		out[2] = 0.0259040371 * l_ + 0.7827717662 * m_ - 0.8086757660 * s_;

		return out;
	}

	color::color_oklch_t color::linear_rgb_t::to_oklch() const
	{
		return to_oklab().to_oklch();
	}

	color::color_hsv_t color::linear_rgb_t::to_hsv() const
	{
		using T         = float;
		const auto self = static_cast<vec3>(*this);
		const T    r    = (self)[0], g = (self)[1], b = (self)[2];

		const T max   = std::max({r, g, b});
		const T min   = std::min({r, g, b});
		const T delta = max - min;

		T h = static_cast<T>(0);
		const T s = max == static_cast<T>(0) ? static_cast<T>(0) : delta / max;
		const T v = max;

		if (delta != static_cast<T>(0))
		{
			if (max == r)
			{
				h = static_cast<T>(60) * std::fmod(((g - b) / delta), static_cast<T>(6));
			} else if (max == g)
			{
				h = static_cast<T>(60) * (((b - r) / delta) + static_cast<T>(2));
			} else // max == b
			{
				h = static_cast<T>(60) * (((r - g) / delta) + static_cast<T>(4));
			}
			if (h < static_cast<T>(0))
				h += static_cast<T>(360);
		}
		return vec3{h, s, v};
	}

	color::linear_rgb_t color::srgb_t::to_linear_rgb() const
	{
		auto copy = static_cast<vec3>(*this);
		copy[0]   = srgb_to_linear(copy[0]);
		copy[1]   = srgb_to_linear(copy[1]);
		copy[2]   = srgb_to_linear(copy[2]);
		return copy;
	}

	color::srgb_t color::srgb_t::to_srgb() const
	{
		return *this;
	}

	color::oklab_t color::srgb_t::to_oklab() const
	{
		return to_linear_rgb().to_oklab();
	}

	color::color_oklch_t color::srgb_t::to_oklch() const
	{
		return to_linear_rgb().to_oklab().to_oklch();
	}

	color::color_hsv_t color::srgb_t::to_hsv() const
	{
		return to_linear_rgb().to_hsv();
	}

	color::linear_rgb_t color::oklab_t::to_linear_rgb() const
	{
		auto copy = static_cast<vec3>(*this);
		/* 1. OKLab -> non-linear LMS′ */
		const double l_ = copy.r() + 0.3963377774 * copy.g() + 0.2158037573 * copy.b();
		const double m_ = copy.r() - 0.1055613458 * copy.g() - 0.0638541728 * copy.b();
		const double s_ = copy.r() - 0.0894841775 * copy.g() - 1.2914855480 * copy.b();

		/* 2. Undo cube root */
		const double l = l_ * l_ * l_;
		const double m = m_ * m_ * m_;
		const double s = s_ * s_ * s_;

		/* 3. LMS -> linear RGB */
		double R = 4.0767416621 * l - 3.3077115913 * m + 0.2309699292 * s;
		double G = -1.2684380046 * l + 2.6097574011 * m - 0.3413193965 * s;
		double B = -0.0041960863 * l - 0.7034186147 * m + 1.7076147010 * s;

		/* 4. Optional: clamp to [0,1] before gamma */
		R = std::clamp(R, 0.0, 1.0);
		G = std::clamp(G, 0.0, 1.0);
		B = std::clamp(B, 0.0, 1.0);

		/* 5. Linear RGB -> sRGB */
		copy[0] = R;
		copy[1] = G;
		copy[2] = B;
		return copy;
	}

	color::srgb_t color::oklab_t::to_srgb() const
	{
		return to_linear_rgb().to_srgb();
	}

	color::oklab_t color::oklab_t::to_oklab() const
	{
		return *this;
	}

	color::color_oklch_t color::oklab_t::to_oklch() const
	{
		const auto self = static_cast<vec3>(*this);
		vec3 out;
		out[0] = self.r();
		out[1] = std::sqrt(self.g() * self.g() + self.b() * self.b());
		out[2] = toDegrees(std::atan2(self.b(), self.g()));
		return out;
	}

	color::color_hsv_t color::oklab_t::to_hsv() const
	{
		return to_linear_rgb().to_hsv();
	}

	color::linear_rgb_t color::color_oklch_t::to_linear_rgb() const
	{
		return to_oklab().to_linear_rgb();
	}

	color::srgb_t color::color_oklch_t::to_srgb() const
	{
		return to_oklab().to_linear_rgb().to_srgb();;
	}

	color::oklab_t color::color_oklch_t::to_oklab() const
	{
		const auto self = static_cast<vec3>(*this);
		vec3 out;
		out[0] = self.r();
		out[1] = self.g() * std::cos(toRadians(self.b()));
		out[2] = self.g() * std::sin(toRadians(self.b()));
		return out;
	}

	color::color_oklch_t color::color_oklch_t::to_oklch() const
	{
		return *this;
	}

	color::color_hsv_t color::color_oklch_t::to_hsv() const
	{
		return to_oklab().to_linear_rgb().to_hsv();
	}

	color::linear_rgb_t color::color_hsv_t::to_linear_rgb() const
	{
		using T = float;
		const auto self = static_cast<vec3>(*this);
		T       h = (self)[0];
		const T s = (self)[1];
		const T v = (self)[2];

		if (s == static_cast<T>(0))
			return vec3{v, v, v}; // achromatic

		h = std::fmod(h, static_cast<T>(360));
		if (h < static_cast<T>(0))
			h += static_cast<T>(360);
		h /= static_cast<T>(60);            // into sector 0-5
		const int i = static_cast<int>(std::floor(h));
		const T   f = h - static_cast<T>(i);

		const T p = v * (static_cast<T>(1) - s);
		const T q = v * (static_cast<T>(1) - s * f);
		const T t = v * (static_cast<T>(1) - s * (static_cast<T>(1) - f));

		switch (i)
		{
			case 0:
				return vec3{v, t, p};
			case 1:
				return vec3{q, v, p};
			case 2:
				return vec3{p, v, t};
			case 3:
				return vec3{p, q, v};
			case 4:
				return vec3{t, p, v};
			default:
				return vec3{v, p, q};
		}
	}

	color::srgb_t color::color_hsv_t::to_srgb() const
	{
		return to_linear_rgb().to_srgb();
	}

	color::oklab_t color::color_hsv_t::to_oklab() const
	{
		return to_linear_rgb().to_oklab();
	}

	color::color_oklch_t color::color_hsv_t::to_oklch() const
	{
		return to_linear_rgb().to_oklab().to_oklch();
	}

	color::color_hsv_t color::color_hsv_t::to_hsv() const
	{
		return *this;
	}
}

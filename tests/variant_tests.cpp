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
#include <blt/std/variant.h>
#include <blt/logging/logging.h>
#include <blt/std/assert.h>

struct base_type
{
	[[nodiscard]] virtual int simple() const = 0;
	[[nodiscard]] virtual std::string to_string() const = 0;

	virtual ~base_type() = default;
};

struct mutate_type : base_type
{
	virtual void mutate(int i) = 0;
};

struct type1 final : base_type
{
	[[nodiscard]] int simple() const final // NOLINT
	{
		return 1;
	}

	[[nodiscard]] std::string to_string() const final // NOLINT
	{
		return "Type1";
	}
};


struct type2 final : base_type
{
	[[nodiscard]] int simple() const final // NOLINT
	{
		return 2;
	}

	[[nodiscard]] std::string to_string() const final // NOLINT
	{
		return "Type2";
	}
};


struct type3 final : base_type
{
	[[nodiscard]] int simple() const final // NOLINT
	{
		return 3;
	}

	[[nodiscard]] std::string to_string() const final // NOLINT
	{
		return "Type3";
	}
};

struct storing_type1 final : mutate_type
{
	explicit storing_type1(const int i): internal(i)
	{
	}

	[[nodiscard]] int simple() const final // NOLINT
	{
		return internal;
	}

	void mutate(const int i) final
	{
		internal = i;
	}

	[[nodiscard]] std::string to_string() const final // NOLINT
	{
		return "Storing Type: {" + std::to_string(internal) + "}";
	}

	int internal;
};

struct storing_type2 final : mutate_type
{
	explicit storing_type2(const float i): internal(i * 2.2534f)
	{
	}

	[[nodiscard]] int simple() const final // NOLINT
	{
		return static_cast<int>(internal);
	}

	void mutate(const int i) final
	{
		internal = static_cast<float>(i) * 2.2534f;
	}

	[[nodiscard]] std::string to_string() const final // NOLINT
	{
		return "Storing Type: {" + std::to_string(internal) + "}";
	}

	float internal;
};

struct no_members
{
	int hello;
};

int main()
{
	blt::variant_t<type1, type2, type3> v1{type1{}};
	blt::variant_t<type1, type2, type3> v2{type2{}};
	blt::variant_t<type1, type2, type3> v3{type3{}};

	BLT_TRACE("Variants to_string():");

	auto v1_result = v1.call_member(&base_type::to_string);
	BLT_ASSERT_MSG(v1_result == type1{}.to_string(), ("Expected result to be " + type1{}.to_string() + " but found " + v1_result).c_str());
	BLT_ASSERT_MSG(typeid(v1_result) == typeid(std::string), "Result type expected to be string!");
	BLT_TRACE("V1: {}", v1_result);

	auto v2_result = v2.call_member(&base_type::to_string);

	BLT_TRACE("V2: {}", v2_result);
	BLT_TRACE("V3: {}", v3.call_member(&base_type::to_string));

	blt::variant_t<type1, type2, no_members> member_missing_stored_member{type1{}};
	blt::variant_t<type1, type2, no_members> member_missing_stored_no_member{no_members{50}};

	auto stored_member_result = member_missing_stored_member.call_member(&base_type::to_string);
	auto no_member_result = member_missing_stored_no_member.call_member(&base_type::to_string);

	BLT_TRACE("Stored: has value? '{}' value: '{}'", stored_member_result.has_value(), *stored_member_result);
	BLT_TRACE("No Member: {}", no_member_result.has_value());
}
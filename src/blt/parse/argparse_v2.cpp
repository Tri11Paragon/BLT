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
#include <blt/parse/argparse_v2.h>
#include <blt/std/assert.h>

namespace blt::argparse
{
    namespace detail
    {

    }


    namespace detail
    {
        // Unit Tests for class argument_string_t
        // Test Case 1: Ensure the constructor handles flags correctly
        void test_argument_string_t_flag_basic(const hashset_t<char>& prefixes)
        {
            const argument_string_t arg("-f", prefixes);
            BLT_ASSERT(arg.is_flag() && "Expected argument to be identified as a flag.");
            BLT_ASSERT(arg.value() == "f" && "Flag value should match the input string.");
        }

        // Test Case 2: Ensure the constructor handles long flags correctly
        void test_argument_string_t_long_flag(const hashset_t<char>& prefixes)
        {
            const argument_string_t arg("--file", prefixes);
            BLT_ASSERT(arg.is_flag() && "Expected argument to be identified as a flag.");
            BLT_ASSERT(arg.value() == "file" && "Long flag value should match the input string.");
        }

        // Test Case 3: Ensure positional arguments are correctly identified
        void test_argument_string_t_positional_argument(const hashset_t<char>& prefixes)
        {
            const argument_string_t arg("filename.txt", prefixes);
            BLT_ASSERT(!arg.is_flag() && "Expected argument to be identified as positional.");
            BLT_ASSERT(arg.value() == "filename.txt" && "Positional argument value should match the input string.");
        }

        // Test Case 5: Handle an empty string
        void test_argument_string_t_empty_input(const hashset_t<char>& prefixes)
        {
            const argument_string_t arg("", prefixes);
            BLT_ASSERT(!arg.is_flag() && "Expected an empty input to be treated as positional, not a flag.");
            BLT_ASSERT(arg.value().empty() && "Empty input should have an empty value.");
        }

        // Test Case 6: Handle edge case of a single hyphen (`-`) which might be ambiguous
        void test_argument_string_t_single_hyphen(const hashset_t<char>& prefixes)
        {
            const argument_string_t arg("-", prefixes);
            BLT_ASSERT(arg.is_flag() && "Expected single hyphen (`-`) to be treated as a flag.");
            BLT_ASSERT(arg.value().empty() && "Single hyphen flag should have empty value.");
            BLT_ASSERT(arg.get_flag() == "-" && "Single hyphen flag should match the input string.");
        }

        // Test Case 8: Handle arguments with prefix only (like "--")
        void test_argument_string_t_double_hyphen(const hashset_t<char>& prefixes)
        {
            const argument_string_t arg("--", prefixes);
            BLT_ASSERT(arg.is_flag() && "Double hyphen ('--') should be treated as a flag.");
            BLT_ASSERT(arg.value().empty() && "Double hyphen flag should have empty value.");
            BLT_ASSERT(arg.get_flag() == "--" && "Double hyphen value should match the input string.");
        }

        // Test Case 9: Validate edge case of an argument with spaces
        void test_argument_string_t_with_spaces(const hashset_t<char>& prefixes)
        {
            const argument_string_t arg("  ", prefixes);
            BLT_ASSERT(!arg.is_flag() && "Arguments with spaces should not be treated as flags.");
            BLT_ASSERT(arg.value() == "  " && "Arguments with spaces should match the input string.");
        }

        // Test Case 10: Validate arguments with numeric characters
        void test_argument_string_t_numeric_flag(const hashset_t<char>& prefixes)
        {
            const argument_string_t arg("-123", prefixes);
            BLT_ASSERT(arg.is_flag() && "Numeric flags should still be treated as flags.");
            BLT_ASSERT(arg.value() == "123" && "Numeric flag value should match the input string.");
        }

        // Test Case 11: Ensure the constructor handles '+' flag correctly
        void test_argument_string_t_plus_flag_basic(const hashset_t<char>& prefixes)
        {
            const argument_string_t arg("+f", prefixes);
            BLT_ASSERT(arg.is_flag() && "Expected argument to be identified as a flag.");
            BLT_ASSERT(arg.value() == "f" && "Plus flag value should match the input string.");
        }

        // Test Case 13: Handle edge case of a single plus (`+`) which might be ambiguous
        void test_argument_string_t_single_plus(const hashset_t<char>& prefixes)
        {
            const argument_string_t arg("+", prefixes);
            BLT_ASSERT(arg.is_flag() && "Expected single plus (`+`) to be treated as a flag.");
            BLT_ASSERT(arg.value().empty() && "Single plus flag should have empty value.");
            BLT_ASSERT(arg.get_flag() == "+" && "Single plus flag should match the input string.");
        }

        // Test Case 14: Handle arguments with prefix only (like '++')
        void test_argument_string_t_double_plus(const hashset_t<char>& prefixes)
        {
            const argument_string_t arg("++", prefixes);
            BLT_ASSERT(arg.is_flag() && "Double plus ('++') should be treated as a flag.");
            BLT_ASSERT(arg.value().empty() && "Double plus flag should have empty value.");
            BLT_ASSERT(arg.get_flag() == "++" && "Double plus value should match the input string.");
        }

        void run_all_tests_argument_string_t()
        {
            const hashset_t<char> prefixes = {'-', '+'};
            test_argument_string_t_flag_basic(prefixes);
            test_argument_string_t_long_flag(prefixes);
            test_argument_string_t_positional_argument(prefixes);
            test_argument_string_t_empty_input(prefixes);
            test_argument_string_t_single_hyphen(prefixes);
            test_argument_string_t_double_hyphen(prefixes);
            test_argument_string_t_with_spaces(prefixes);
            test_argument_string_t_numeric_flag(prefixes);
            test_argument_string_t_plus_flag_basic(prefixes);
            test_argument_string_t_single_plus(prefixes);
            test_argument_string_t_double_plus(prefixes);
        }

        void test()
        {
            run_all_tests_argument_string_t();
        }
    }
}

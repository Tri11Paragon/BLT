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
#include <iostream>
#include <sstream>
#include <blt/logging/logging.h>
#include <blt/std/assert.h>
#include <blt/std/utility.h>

struct some_silly_type_t
{
};

auto expected_str = std::string(R"(This is a println!
This is a println with args '42'
This is a println with multiple args '42' '32.342311859130859375' 'Hello World!'
This is a 'Well so am I except cooler :3' fmt string with positionals 'I am a string!'
This is a println with a sign +4120
This is a println with a sign -4120
This is a println with a space  4120
This is a println with a space -4120
This is a println with a minus 4120
This is a println with a minus -4120
This is a println with a with       4120
This is a println with a with leading zeros 0000004120
This is a println with a precision 42.2323423490
This is a println with hex 109a
This is a println with hex with leading 0x109a
This is a println with binary 0b00110010000110100101011000000000
This is a println with binary with space 0b10110010 00011010 01010110 00000000
This is a println with binary with space 10100010 00000000 00000000 00000000
This is a println with octal 015015
This is a println with hexfloat 0x1.926e978d4fdf4p+8
This is a println with exponent 4.4320902431999996e+07
This is a println with exponent 9.5324342340423400e+15
This is a println with general 953243.49
This is a println with general 9.532433240234033e+17
This is a println with a char B
This is a println with type some_silly_type_t
This is a println with boolean true
This is a println with boolean as int 0
This is a println with boolean as hex 0x1
This is a println with boolean as octal 1
This is a println with alignment left 64         end value
This is a println with alignment right         46 end value
This is a println with alignment left (fill) 46******** end value
This is a println with alignment right (fill) ********46 end value
)");

std::pair<bool, std::string> compare_strings(const std::string& s1, const std::string& s2)
{
    if (s1.size() != s2.size())
        return {false, "Strings size do not match '" + std::to_string(s1.size()) + "' vs '" + std::to_string(s2.size()) + "'"};
    size_t index = 0;
    for (; index < s1.size(); ++index)
    {
        if (s1[index] != s2[index])
        {
            std::stringstream ss;
            const auto i1 = std::max(static_cast<blt::i64>(index) - 32, 0l);
            const auto l1 = std::min(static_cast<blt::i64>(s1.size()) - i1, 65l);
            ss << "Strings differ at index " << index << "!\n";
            ss << "'" << s1.substr(i1, l1) << "' vs '" << s2.substr(i1, l1) << "'" << std::endl;
            return {false, ss.str()};
        }
    }
    return {true, ""};
}

int main()
{
    std::stringstream ss;
    blt::logging::println(ss, "This is a println!");
    blt::logging::println(ss, "This is a println with args '{}'", 42);
    blt::logging::println(ss, "This is a println with multiple args '{}' '{:.100}' '{}'", 42, 32.34231233f, "Hello World!");
    blt::logging::println(ss, "This is a '{1}' fmt string with positionals '{0}'", "I am a string!", "Well so am I except cooler :3");
    blt::logging::println(ss, "This is a println with a sign {:+}", 4120);
    blt::logging::println(ss, "This is a println with a sign {:+}", -4120);
    blt::logging::println(ss, "This is a println with a space {: }", 4120);
    blt::logging::println(ss, "This is a println with a space {: }", -4120);
    blt::logging::println(ss, "This is a println with a minus {:-}", 4120);
    blt::logging::println(ss, "This is a println with a minus {:-}", -4120);
    blt::logging::println(ss, "This is a println with a with {:10}", 4120);
    blt::logging::println(ss, "This is a println with a with leading zeros {:010}", 4120);
    blt::logging::println(ss, "This is a println with a precision {:.10f}", 42.232342349);
    blt::logging::println(ss, "This is a println with hex {:.10x}", 4250);
    blt::logging::println(ss, "This is a println with hex with leading {:#.10x}", 4250);
    blt::logging::println(ss, "This is a println with binary {:#b}", 6969420);
    blt::logging::println(ss, "This is a println with binary with space {: #b}", 6969421);
    blt::logging::println(ss, "This is a println with binary with space {: b}", 69);
    blt::logging::println(ss, "This is a println with octal {:#o}", 6669);
    blt::logging::println(ss, "This is a println with hexfloat {:a}", 402.4320);
    blt::logging::println(ss, "This is a println with exponent {:e}", 44320902.4320);
    blt::logging::println(ss, "This is a println with exponent {:e}", 9532434234042340.0);
    blt::logging::println(ss, "This is a println with general {:g}", 953243.49);
    blt::logging::println(ss, "This is a println with general {:g}", 953243324023403240.49);
    blt::logging::println(ss, "This is a println with a char {:c}", 66);
    blt::logging::println(ss, "This is a println with type {:t}", some_silly_type_t{});
    blt::logging::println(ss, "This is a println with boolean {}", true);
    blt::logging::println(ss, "This is a println with boolean as int {:d}", false);
    blt::logging::println(ss, "This is a println with boolean as hex {:#x}", true);
    blt::logging::println(ss, "This is a println with boolean as octal {:o}", true);
    blt::logging::println(ss, "This is a println with alignment left {:<10} end value", 64);
    blt::logging::println(ss, "This is a println with alignment right {:>10} end value", 46);
    blt::logging::println(ss, "This is a println with alignment left (fill) {:*<10} end value", 46);
    blt::logging::println(ss, "This is a println with alignment right (fill) {:*>10} end value", 46);
    blt::logging::print(ss.str());
    auto [passed, error_msg] = compare_strings(expected_str, ss.str());
    BLT_ASSERT_MSG(passed && "Logger logged string doesn't match precomputed expected string!", error_msg.c_str());

    // blt::logging::println("This is println {}\twith a std::endl in the middle of it");
}

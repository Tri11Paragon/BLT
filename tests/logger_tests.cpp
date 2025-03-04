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
#include <blt/logging/logging.h>
#include <blt/std/utility.h>

struct some_silly_type_t
{
};

int main()
{
    blt::logging::println("This is a println!");
    blt::logging::println("This is a println with args '{}'", 42);
    blt::logging::println("This is a println with multiple args '{}' '{:.100}' '{}'", 42, 32.34231233f, "Hello World!");
    blt::logging::println("This is a '{1}' fmt string with positionals '{0}'", "I am a string!", "Well so am I except cooler :3");
    blt::logging::println("This is a println with a sign {:+}", 4120);
    blt::logging::println("This is a println with a sign {:+}", -4120);
    blt::logging::println("This is a println with a space {: }", 4120);
    blt::logging::println("This is a println with a space {: }", -4120);
    blt::logging::println("This is a println with a minus {:-}", 4120);
    blt::logging::println("This is a println with a minus {:-}", -4120);
    blt::logging::println("This is a println with a with {:10}", 4120);
    blt::logging::println("This is a println with a with leading zeros {:010}", 4120);
    blt::logging::println("This is a println with a precision {:.10f}", 42.232342349);
    blt::logging::println("This is a println with hex {:.10x}", 4250);
    blt::logging::println("This is a println with hex with leading {:#.10x}", 4250);
    blt::logging::println("This is a println with binary {:#b}", 6969420);
    blt::logging::println("This is a println with binary with space {: #b}", 6969421);
    blt::logging::println("This is a println with octal {:#o}", 6669);
    blt::logging::println("This is a println with hexfloat {:a}", 402.4320);
    blt::logging::println("This is a println with exponent {:e}", 44320902.4320);
    blt::logging::println("This is a println with exponent {:e}", 9532434234042340);
    blt::logging::println("This is a println with exponent {:g}", 953243.49);
    blt::logging::println("This is a println with exponent {:g}", 953243324023403240.49);
    blt::logging::println("This is a println with a char {:c}", 66);
    blt::logging::println("This is a println with type {:t}", some_silly_type_t{});
    // blt::logging::println("This is println {}\twith a std::endl in the middle of it");
}

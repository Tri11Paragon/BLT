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

int main()
{

    using endl_t = decltype(static_cast<std::ostream& (*)(std::ostream&)>(std::endl));
    // blt::logging::println("{} | {} | {} | {}", blt::type_string<endl_t>());
    blt::logging::println("This is a println!");
    blt::logging::println("This is a println with args '{}'", 42);
    blt::logging::println("This is a println with multiple args '{}' '{}' '{}'", 42, 32.34231233, "Hello World!");
    blt::logging::println("This is a '{1}' fmt string with positionals '{0}'", "I am a string!", "Well so am I except cooler :3");
    // blt::logging::println("This is println {}\twith a std::endl in the middle of it");
}

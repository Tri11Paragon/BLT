/*
 *  BLT C++ 20 / C++ 17 Compatability helper file
 *  Copyright (C) 2023  Brett Terpstra
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

#ifndef INSANE_DNS_COMPATIBILITY_H
#define INSANE_DNS_COMPATIBILITY_H

#if __cplusplus >= 202002L
    #define BLT_CONTAINS(container, value) container.contains(value)
#else
    #include <algorithm>
    #define BLT_CONTAINS(container, value) std::find(container.begin(), container.end(), value) != container.end()
#endif

#endif //INSANE_DNS_COMPATIBILITY_H

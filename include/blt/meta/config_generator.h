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

#ifndef BLT_CONFIG_GENERATOR_H
#define BLT_CONFIG_GENERATOR_H

#include <utility>

namespace blt
{
#define BLT_MAKE_VARIABLE(TYPE, NAME)                                   \
    TYPE NAME{};

#define BLT_INTERNAL_MAKE_GETTER(TYPE, NAME)                            \
    TYPE& get_##NAME() { return NAME; }                                 \
    const TYPE& get_##NAME() const { return NAME; }

#define BLT_INTERNAL_MAKE_SETTER(TYPE, NAME)                            \
    auto& set_##NAME(const TYPE& new_##NAME)                            \
    {                                                                   \
        NAME = new_##NAME;                                              \
        return *this;                                                   \
    }                                                                   \
    auto& set_##NAME(TYPE new_##NAME)                                   \
    {                                                                   \
        NAME = std::move(new_##NAME);                                   \
        return *this;                                                   \
    }

#define BLT_MAKE_GETTER(TYPE, NAME)                                     \
    private:                                                            \
        BLT_MAKE_VARIABLE(TYPE, NAME)                                   \
    public:                                                             \
        BLT_INTERNAL_MAKE_GETTER(TYPE, NAME)

#define BLT_MAKE_SETTER(TYPE, NAME)                                     \
    private:                                                            \
        BLT_MAKE_VARIABLE(TYPE, NAME)                                   \
    public:                                                             \
        BLT_INTERNAL_MAKE_SETTER(TYPE, NAME)

#define BLT_MAKE_FRIEND(FRIEND) friend FRIEND;

//#define BLT_MAKE_ARG(TYPE, NAME) TYPE, NAME
//#define INTERNAL_BLT_ARG_INPUT(TYPE, NAME) TYPE NAME
//#define INTERNAL_BLT_ARG_ASSIGNMENT(TYPE, NAME) _##NAME(NAME)
//#define BLT_MAKE_CONSTRUCTOR(OBJECT, ...) OBJECT()

#define BLT_MAKE_GETTER_AND_SETTER(TYPE, NAME)                          \
    private:                                                            \
        BLT_MAKE_VARIABLE(TYPE, NAME)                                   \
    public:                                                             \
        BLT_INTERNAL_MAKE_GETTER(TYPE, NAME)                            \
        BLT_INTERNAL_MAKE_SETTER(TYPE, NAME)

#define BLT_MAKE_CONFIG_TYPE(OBJECT, ...)                               \
    class OBJECT {                                                      \
        __VA_ARGS__                                                     \
    };                                                                  \

}

#endif //BLT_CONFIG_GENERATOR_H

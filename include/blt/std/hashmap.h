/*
 * Created by Brett on 31/03/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_HASH_MAP_H
#define BLT_HASH_MAP_H

#ifndef HASHMAP
    #if defined __has_include && __has_include(<parallel_hashmap/phmap.h>)
#include <parallel_hashmap/phmap.h>
        #include <parallel_hashmap/phmap_fwd_decl.h>
        template<typename K, typename V>
        using HASHMAP = phmap::flat_hash_map<K, V>;
        template<typename K>
        using HASHSET = phmap::flat_hash_set<K>;
    #else
        #include <unordered_map>
        #include <unordered_set>

template<typename K, typename V>
using HASHMAP = std::unordered_map<K, V>;

template<typename K>
using HASHSET = std::unordered_set<K>;
    #endif
#endif

#endif //BLT_HASH_MAP_H

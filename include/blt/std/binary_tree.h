/*
 * Created by Brett on 09/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#include <stdexcept>
#include <vector>
#include <blt/std/queue.h>
#include <iostream>
#include <memory>

#ifndef BLT_BINARY_TREE_H
#define BLT_BINARY_TREE_H

namespace blt {
    
    class binary_search_tree_error : public std::runtime_error {
        public:
            explicit binary_search_tree_error(const std::string& string): runtime_error(string) {}
    };
    
    
    
}

#endif //BLT_BINARY_TREE_H

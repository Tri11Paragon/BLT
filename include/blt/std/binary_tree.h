/*
 * Created by Brett on 09/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_BINARY_TREE_H
#define BLT_BINARY_TREE_H

#include <stdexcept>
#include <vector>
#include <blt/std/allocator.h>
#include <iostream>
#include <memory>
// TODO: blt::queue
#include <queue>

namespace blt
{
    
    class binary_search_tree_error : public std::runtime_error
    {
        public:
            explicit binary_search_tree_error(const std::string& string): runtime_error(string)
            {}
    };
    
    template<typename T, typename ALLOC = blt::area_allocator<T>>
    class AVL_node_tree
    {
        private:
            struct node
            {
                T val;
                node* left;
                node* right;
                
                node(const T& t): val(t)
                {}
                
                node(T&& m): val(m)
                {}
                
                node(const node& copy) = delete;
                
                node(node&& move) = delete;
                
                node& operator=(const node& copy) = delete;
                
                node& operator=(node&& move) = delete;
                
                ~node()
                {
                    delete left;
                    delete right;
                }
            };
            
            ALLOC alloc;
            node* root = nullptr;
            
            node* newNode(const T& t)
            {
                return alloc.allocate(1);
            }
        
        public:
            AVL_node_tree() = default;
            
            AVL_node_tree(const AVL_node_tree& copy) = delete;
            
            AVL_node_tree(AVL_node_tree&& move) = delete;
            
            AVL_node_tree& operator=(const AVL_node_tree& copy) = delete;
            
            AVL_node_tree& operator=(AVL_node_tree&& move) = delete;
            
            size_t height(node* start = nullptr)
            {
                if (start == nullptr)
                    start = root;
                if (start == nullptr)
                    return 0;
                std::queue<node*> nodes;
                nodes.push(start);
                size_t height = 0;
                while (!nodes.empty())
                {
                    height++;
                    size_t level_count = nodes.size();
                    while (level_count-- > 0)
                    {
                        if (nodes.front()->left != nullptr)
                            nodes.push(nodes.front()->left);
                        if (nodes.front()->right != nullptr)
                            nodes.push(nodes.front()->right);
                        nodes.pop();
                    }
                }
                return height;
            }
            
            void insert(const T& t)
            {
                if (root == nullptr)
                {
                    root = new node(t);
                    return;
                }
                node* search = root;
                node* parent = nullptr;
                while (true)
                {
                    if (t < search->val)
                    {
                        if (search->left == nullptr)
                        {
                            search->left = new node(t);
                            break;
                        }
                        search = search->left;
                    } else
                    {
                        if (search->right == nullptr)
                        {
                            search->right = new node(t);
                            break;
                        }
                        search = search->right;
                    }
                    parent = search;
                }
            }
            
            ~AVL_node_tree()
            {
                delete root;
            }
    };
    
}

#endif //BLT_BINARY_TREE_H

/*
 * Created by Brett on 09/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#include <stdexcept>
#include <vector>
#include <blt/std/queue.h>

#ifndef BLT_BINARY_TREE_H
#define BLT_BINARY_TREE_H

namespace blt {
    
    class binary_search_tree_error : public std::runtime_error {
        public:
            explicit binary_search_tree_error(const std::string& string): runtime_error(string) {}
    };
    
    template<typename T>
    class node_binary_search_tree {
        protected:
            struct BST_node {
                BST_node* left = nullptr;
                BST_node* right = nullptr;
                T payload;
                
                ~BST_node() {
                    delete (left);
                    delete (right);
                }
            };
            
            BST_node* m_root = nullptr;
        private:
            void insert(BST_node* root, const T& element) {
                BST_node* searchNode = root;
                // basically we are iterating through the tree looking for a valid node to insert into.
                while (true) {
                    // check for left and right tree traversal if it exists
                    if (searchNode->left != nullptr && element < searchNode->left->payload) {
                        searchNode = searchNode->left;
                        continue;
                    }
                    if (searchNode->right != nullptr && element > searchNode->right->payload) {
                        searchNode = searchNode->right;
                        continue;
                    }
                    if (element == searchNode->payload)
                        throw binary_search_tree_error{"Unable to insert. Nodes cannot have equal values!\n"};
                    // insert into the lowest node consistent with a BST
                    if (element < searchNode->payload) {
                        searchNode->left = new BST_node();
                        searchNode->left->payload = element;
                    } else {
                        searchNode->right = new BST_node();
                        searchNode->right->payload = element;
                    }
                    return;
                }
            }
            
            BST_node* search(BST_node*& parent, const T& element) {
                BST_node* searchNode = m_root;
                // basically we are iterating through the tree looking for a valid node to insert into.
                while (true) {
                    if (searchNode->payload == element)
                        return searchNode->payload;
                    // check for left and right tree traversal if it exists
                    if (searchNode->left != nullptr && element < searchNode->left->payload) {
                        parent = searchNode;
                        searchNode = searchNode->left;
                        continue;
                    }
                    if (searchNode->right != nullptr && element > searchNode->right->payload) {
                        parent = searchNode;
                        searchNode = searchNode->right;
                        continue;
                    }
                }
            }
            
            std::vector<BST_node*> inOrderTraverse(BST_node* root) {
                std::vector<BST_node*> nodes{};
                blt::flat_stack<BST_node*> nodeStack{};
                
                BST_node* current = root;
                while (current != nullptr || !nodeStack.isEmpty()) {
                    // go all the way to the left subtree
                    while (current != nullptr){
                        nodeStack.push(current);
                        current = current->left;
                    }
                    current = nodeStack.front();
                    nodeStack.pop();
                    nodes.push_back(current);
                    current = current->right;
                }
                
                return nodes;
            }
        
        public:
            node_binary_search_tree() {
                m_root = new BST_node();
            }
            
            void insert(const T& element) {
                insert(m_root, element);
            }
            
            [[nodiscard]] BST_node* search(const T& element) const {
                search(nullptr, element);
            }
            
            void remove(const T& element) {
                BST_node* parent = nullptr;
                auto elementNode = search(parent, element);
                // reconstruct subtree. More efficient way of doing this... TODO
                std::vector<BST_node*> subNodes = inOrderTraverse(elementNode);
                for (auto* node : subNodes){
                    if (node != elementNode)
                        insert(parent, node->payload);
                }
            }
            
            ~node_binary_search_tree() {
                delete (m_root);
            }
    };
    
    template<typename T>
    class flat_binary_search_tree {
        private:
        
    };
    
    template<typename T>
    using node_BST = node_binary_search_tree<T>;
    template<typename T>
    using flat_BST = flat_binary_search_tree<T>;
    
}

#endif //BLT_BINARY_TREE_H

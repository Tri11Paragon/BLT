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
    
    template<typename T>
    class node_binary_search_tree {
        protected:
            struct BST_node {
                BST_node* left = nullptr;
                BST_node* right = nullptr;
                T payload;
                
                explicit BST_node(const T& _payload) {
                    payload = _payload;
                }
            };
            BST_node* m_root = nullptr;
        private:
            void insert(BST_node* root, const T& element) {
                if (root == nullptr)
                    throw binary_search_tree_error{"Unable to insert. Provided root is null!\n"};
                BST_node* searchNode = root;
                // basically we are iterating through the tree looking for a valid node to insert into.
                while (true) {
                    if (element == searchNode->payload)
                        throw binary_search_tree_error{"Unable to insert. Nodes cannot have equal values! (" + std::to_string(element) + ")\n"};
                    // check for left and right tree traversal if it exists
                    if (searchNode->left != nullptr && element < searchNode->payload) {
                        searchNode = searchNode->left;
                        continue;
                    }
                    if (searchNode->right != nullptr && element > searchNode->payload) {
                        searchNode = searchNode->right;
                        continue;
                    }
                    // insert into the lowest node consistent with a BST
                    if (element < searchNode->payload)
                        searchNode->left = new BST_node(element);
                    else
                        searchNode->right = new BST_node(element);
                    return;
                }
            }

            BST_node* search(BST_node** parent, const T& element) const {
                BST_node* searchNode = m_root;
                BST_node* parentNode = m_root;

                if (searchNode->left == nullptr && searchNode->right == nullptr)
                    return nullptr;

                // basically we are iterating through the tree looking for a valid node to insert into.
                while (searchNode->payload != element) {
                    if (searchNode == nullptr)
                        return nullptr;
                    // check for left and right tree traversal if it exists
                    if (element < searchNode->payload) {
                        parentNode = searchNode;
                        searchNode = searchNode->left;
                    } else {
                        parentNode = searchNode;
                        searchNode = searchNode->right;
                    }
                }
                if (parent != nullptr)
                    *parent = parentNode;
                return searchNode;
            }
            
            std::vector<BST_node*> inOrderTraverse(BST_node* root) {
                std::vector<BST_node*> nodes{};
                blt::flat_stack<BST_node*> nodeStack{};
                
                BST_node* current = root;
                while (current != nullptr || !nodeStack.isEmpty()) {
                    // go all the way to the left subtree
                    while (current != nullptr) {
                        nodeStack.push(current);
                        current = current->left;
                    }
                    // take the parent node of the left most subtree
                    current = nodeStack.top();
                    nodeStack.pop();
                    nodes.push_back(current);
                    // traverse its right tree
                    current = current->right;
                }
                
                return nodes;
            }

            BST_node*& findMin(BST_node* root) {
                BST_node*& searchNode = root;
                while (searchNode->left != nullptr)
                    searchNode = searchNode->left;
                return searchNode;
            }

            BST_node*& findMax(BST_node* root) {
                BST_node*& searchNode = root;
                while (searchNode->right != nullptr)
                    searchNode = searchNode->right;
                return searchNode;
            }

            BST_node* remove(BST_node* root, const T& element) {
                if (root->payload < element) // search left
                    root->left = remove(root->left, element);
                else if (root->payload > element) // search right
                    root->right = remove(root->right, element);
                else {
                    if (root->left != nullptr && root->right != nullptr) {
                        root->payload = findMin(root->right)->payload;
                        root->right = remove(root->right, root->payload);
                    }
                }
                return root;
            }
        public:
            node_binary_search_tree() = default;
            
            inline void insert(const T& element) {
                if (m_root == nullptr) {
                    m_root = new BST_node(element);
                    return;
                }
                insert(m_root, element);
            }
            
            [[nodiscard]] inline BST_node* search(const T& element) const {
                return search(nullptr, element);
            }
            
            void remove(const T& element) {
                BST_node* parent = nullptr;
                BST_node* elementNode = search(&parent, element);
                if (parent == elementNode)
                    parent = nullptr;
                
                if (elementNode->left != nullptr && elementNode->right != nullptr) {
                    auto traverseNodes = inOrderTraverse(elementNode);
                    if (parent == nullptr) {
                        m_root = nullptr;
                    } else {
                        if (parent->right == elementNode)
                            parent->right = nullptr;
                        else if (parent->left == elementNode)
                            parent->left = nullptr;
                        else
                            throw binary_search_tree_error("Parent node doesn't own child!\n");
                    }
                    for (auto* node : traverseNodes) {
                        if (node != elementNode) {
                            if (parent == nullptr) {
                                insert(node->payload);
                            } else
                                insert(parent, node->payload);
                            delete(node);
                        }
                    }
                    /*BST_node* inOrderSuccessor = elementNode->right;
                    BST_node* inOrderSuccessorParent = nullptr;
                    while (true){
                        // go all the way to the left subtree
                        while (inOrderSuccessor->left != nullptr) {
                            inOrderSuccessorParent = inOrderSuccessor;
                            inOrderSuccessor = inOrderSuccessor->left;
                        }
                        if (inOrderSuccessor->right != nullptr) {
                            inOrderSuccessorParent = inOrderSuccessor;
                            inOrderSuccessor = inOrderSuccessor->right;
                        } else
                            break;
                    }

                    if (parent != nullptr) {
                        if (parent->right == elementNode)
                            parent->right = inOrderSuccessor;
                        else if (parent->left == elementNode)
                            parent->left = inOrderSuccessor;
                        else
                            throw binary_search_tree_error("Parent node doesn't own child!\n");
                    } else
                        m_root = inOrderSuccessor;
                    // reconstruct the node's children
                    inOrderSuccessor->left = elementNode->left;
                    inOrderSuccessor->right = elementNode->right;
                    // delete the parent's reference to the moved node
                    if (inOrderSuccessorParent != nullptr) {
                        if (inOrderSuccessorParent->left == inOrderSuccessor)
                            inOrderSuccessorParent->left = nullptr;
                        else if (inOrderSuccessorParent->right == inOrderSuccessor)
                            inOrderSuccessorParent->right = nullptr;
                        else
                            throw binary_search_tree_error("Parent does not contain child!\n");
                    }
                    rebalance(parent);*/
                } else {
                    auto replacementNode = elementNode->left != nullptr ? elementNode->left : elementNode->right;
                    if (parent == nullptr)
                        m_root = replacementNode;
                    else {
                        if (parent->right == elementNode)
                            parent->right = replacementNode;
                        else if (parent->left == elementNode)
                            parent->left = replacementNode;
                        else
                            throw binary_search_tree_error("Parent node doesn't contain element of search!\n");
                    }
                }
                delete (elementNode);
            }
            
            inline std::vector<BST_node*> inOrderTraverse() {
                return inOrderTraverse(m_root);
            }
            
            inline BST_node* debug() {
                return m_root;
            }
            
            ~node_binary_search_tree() {
                auto inOrder = inOrderTraverse();
                for (auto* n : inOrder)
                    delete(n);
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

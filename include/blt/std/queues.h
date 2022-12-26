/*
 * Created by Brett on 26/12/22.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_QUEUES_H
#define BLT_QUEUES_H

namespace BLT {
    
    template<typename T>
    struct node {
        T t;
        node* next;
    };
    
    template<typename T>
    class flat_queue {
    
    };
    
    template<typename T>
    class node_queue {
        private:
            node<T>* head;
        public:
        
            void insert(const T& t) {
                auto newNode = new node<T>(t, nullptr);
                if (head == nullptr)
                    head = newNode;
                else {
                    newNode->next = head;
                    head = newNode;
                }
            }
        
            [[nodiscard]] const T& front() const {
                return head->t;
            }
        
            void pop() {
                auto nextNode = head->next;
                delete(head);
                head = nextNode;
            }
        
            ~node_queue() {
                auto next = head;
                while (next != nullptr){
                    auto nextNode = next->next;
                    delete(next);
                    next = nextNode;
                }
            }
    };
    
}

#endif //BLT_QUEUES_H

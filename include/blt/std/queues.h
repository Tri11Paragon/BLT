/*
 * Created by Brett on 26/12/22.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_QUEUES_H
#define BLT_QUEUES_H

/**
 * Do no use any queue in this file. They are slower than std::queue.
 */
namespace BLT {
    
    template<typename T>
    struct node {
        T t;
        node* next;
        node(const T& t, node* next){
            this->t = t;
            this->next = next;
        }
    };
    
    template<typename T>
    class flat_queue {
        private:
            int size = 16;
            int insertIndex = 0;
            int headIndex = 0;
            T* data = new T[size];
            
            /**
             * Expands the internal array to the new size, copying over the data and shifting its minimal position to index 0
             * and deletes the old array from memory.
             * @param newSize new size of the internal array
             */
            void expand(int newSize){
                auto tempData = new T[newSize];
                for (int i = 0; i < size - headIndex; i++)
                    tempData[i] = data[i + headIndex];
                delete[] data;
                insertIndex = size - headIndex;
                headIndex = 0;
                data = tempData;
                size = newSize;
            }
            
        public:
        
            void push(const T& t) {
                if (insertIndex >= size){
                    expand(size * 2);
                }
                data[insertIndex++] = t;
            }
        
            /**
             * Warning does not contain runtime error checking!
             * @return the element at the "front" of the queue.
             */
            [[nodiscard]] const T& front() const {
                return data[headIndex];
            }
        
            void pop() {
                // TODO: throw exception when popping would result in a overflow?
                // I didn't make it an exception here due to not wanting to import the class.
                if (headIndex >= size)
                    return;
                headIndex++;
            }
            
            bool isEmpty(){
                return headIndex >= size;
            }
        
            ~flat_queue() {
                delete[](data);
            }
    };
    
    // avoid this. it is very slow.
    template<typename T>
    class node_queue {
        private:
            node<T>* head;
        public:
        
            void push(const T& t) {
                if (head == nullptr)
                    head = new node<T>(t, nullptr);
                else
                    head = new node<T>(t, head);
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

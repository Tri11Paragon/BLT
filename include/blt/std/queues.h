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
        
        node(const T& t, node* next) {
            this->t = t;
            this->next = next;
        }
    };
    
    template<typename T>
    class flat_queue {
        private:
            int m_size = 16;
            int m_insertIndex = 0;
            T* m_data = new T[m_size];
            
            /**
             * Expands the internal array to the new size, copying over the data and shifting its minimal position to index 0
             * and deletes the old array from memory.
             * @param newSize new size of the internal array
             */
            void expand(int newSize) {
                auto tempData = new T[newSize];
                for (int i = 0; i < m_insertIndex; i++)
                    tempData[i] = m_data[i];
                delete[] m_data;
                m_data = tempData;
                m_size = newSize;
            }
        
        public:
            
            void push(const T& t) {
                if (m_insertIndex >= m_size) {
                    expand(m_size * 2);
                }
                m_data[m_insertIndex++] = t;
            }
            
            /**
             * Warning does not contain runtime error checking!
             * @return the element at the "front" of the queue.
             */
            [[nodiscard]] const T& front() const {
                return m_data[m_insertIndex - 1];
            }
            
            void pop() {
                // TODO: throw exception when popping would result in a overflow?
                // I didn't make it an exception here due to not wanting to import the class.
                if (isEmpty())
                    return;
                m_insertIndex--;
            }
            
            bool isEmpty() {
                return m_insertIndex <= 0;
            }
            
            int size() {
                return m_insertIndex;
            }
            
            ~flat_queue() {
                delete[](m_data);
            }
    };
    
    // avoid this. it is very slow.
    template<typename T>
    class node_queue {
        private:
            node<T>* m_head;
        public:
            
            void push(const T& t) {
                if (m_head == nullptr)
                    m_head = new node<T>(t, nullptr);
                else
                    m_head = new node<T>(t, m_head);
            }
            
            [[nodiscard]] const T& front() const {
                return m_head->t;
            }
            
            void pop() {
                auto nextNode = m_head->next;
                delete (m_head);
                m_head = nextNode;
            }
            
            ~node_queue() {
                auto next = m_head;
                while (next != nullptr) {
                    auto nextNode = next->next;
                    delete (next);
                    next = nextNode;
                }
            }
    };
    
}

#endif //BLT_QUEUES_H

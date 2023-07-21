/*
 * Created by Brett on 26/12/22.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_QUEUE_H
#define BLT_QUEUE_H

/**
 *
 */
namespace blt {
    
    /**
     * Standard array backed first in first out queue
     * @tparam T type stored in the queue
     */
    template<typename T>
    class flat_stack {
        private:
            int m_size = 16;
            int m_insertIndex = 0;
            T* m_data = new T[m_size];
            
            /**
             * Expands the internal array to the new size, copying over the data and shifting its minimal position to index 0
             * and deletes the old array from memory.
             * @param newSize new size of the internal array
             */
            void expand() {
                int new_size = m_size * 2;
                auto tempData = new T[new_size];
                for (int i = 0; i < m_insertIndex; i++)
                    tempData[i] = m_data[i];
                delete[] m_data;
                m_data = tempData;
                m_size = new_size;
            }
        
        public:
            
            void push(const T& t) {
                if (m_insertIndex >= m_size) {
                    expand();
                }
                m_data[m_insertIndex++] = t;
            }
            
            /**
             * Warning does not contain runtime error checking!
             * @return the element at the "front" of the queue.
             */
            [[nodiscard]] const T& top() const {
                return m_data[m_insertIndex - 1];
            }
            
            void pop() {
                if (empty())
                    return;
                m_insertIndex--;
            }
        
            [[nodiscard]] inline bool empty() const {
                return m_insertIndex <= 0;
            }
        
            [[nodiscard]] inline int size() const {
                return m_insertIndex;
            }
            
            ~flat_stack() {
                delete[](m_data);
            }
    };
    
    /**
     * Standard array backed first in last out queue (stack)
     * @tparam T type stored in the queue
     */
    template<typename T>
    class flat_queue {
        private:
            int m_size = 16;
            int m_headIndex = 0;
            int m_insertIndex = 0;
            T* m_data;
            /**
             * Expands the internal array to allow for more storage of elements
             */
            void expand() {
                int new_size = m_size * 2;
                int removed_size = m_size - m_headIndex;
                auto tempData = new T[new_size];
                // only copy data from where we've removed onward
                for (int i = 0; i < removed_size; i++)
                    tempData[i] = m_data[i + m_headIndex]; // but don't copy data we've pop'd
                delete[] m_data;
                m_headIndex = 0;
                m_insertIndex = removed_size - 1;
                m_data = tempData;
                m_size = new_size;
            }
        
        public:
            flat_queue(): m_data(new T[m_size]) {
            
            }
        
            inline void push(const T& t) {
                if (m_insertIndex+1 >= m_size) {
                    expand();
                }
                m_data[m_insertIndex++] = t;
            }
            
            /**
             * Warning does not contain runtime error checking!
             * @return the element at the "front" of the queue.
             */
            [[nodiscard]] const T& front() const {
                return m_data[m_headIndex];
            }
        
            inline void pop() {
                if (empty())
                    return;
                m_headIndex++;
            }
            
            [[nodiscard]] inline bool empty() const {
                return m_headIndex >= m_size;
            }
            
            [[nodiscard]] inline int size() const {
                return m_insertIndex - m_headIndex;
            }
        
            inline T* begin(){
                return m_data[m_headIndex];
            }
        
            inline T* end(){
                return m_data[m_insertIndex];
            }
            
            ~flat_queue() {
                delete[](m_data);
            }
    };
}

#endif //BLT_QUEUE_H

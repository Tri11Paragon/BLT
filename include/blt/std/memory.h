/*
 * Created by Brett on 08/02/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_TESTS_MEMORY_H
#define BLT_TESTS_MEMORY_H

namespace blt {
/**
     * Creates an encapsulation of a T array which will be automatically deleted when this object goes out of scope.
     * This is a simple buffer meant to be used only inside of a function and not moved around, with a few minor exceptions.
     * The internal buffer is allocated on the heap.
     * The operator * has been overloaded to return the internal buffer. (or just use scoped_buffer.buffer if you wish to be explicit)
     * The operator & was not used because I think it is stupid to do so.
     * "*" on a reference is fine however since it isn't used to dereference in the case.
     * @tparam T type that is stored in buffer eg char
     */
    template<typename T>
    struct scoped_buffer {
        T* buffer;
        unsigned long size;
        
        explicit scoped_buffer(unsigned long size): size(size) {
            buffer = new T[size];
        }
        
        scoped_buffer(scoped_buffer& copy) = delete;
        
        scoped_buffer(scoped_buffer&& move) = delete;
        
        scoped_buffer operator=(scoped_buffer& copyAssignment) = delete;
        
        scoped_buffer operator=(scoped_buffer&& moveAssignment) = delete;
        
        inline T& operator[](unsigned long index) const {
            return buffer[index];
        }
        
        inline T* operator*(){
            return buffer;
        }
        
        ~scoped_buffer() {
            delete[] buffer;
        }
    };
    
    template<typename T>
    struct nullptr_initializer {
        private:
            T* m_ptr = nullptr;
        public:
            nullptr_initializer() = default;
            explicit nullptr_initializer(T* ptr): m_ptr(ptr) {}
            nullptr_initializer(const nullptr_initializer<T>& ptr): m_ptr(ptr.m_ptr) {}
            nullptr_initializer(nullptr_initializer<T>&& ptr) noexcept : m_ptr(ptr.m_ptr) {}
            
            nullptr_initializer<T>& operator=(const nullptr_initializer<T>& ptr){
                if (&ptr == this)
                    return *this;
                this->m_ptr = ptr.m_ptr;
                return *this;
            }
            
            nullptr_initializer<T>& operator=(nullptr_initializer<T>&& ptr) noexcept {
                if (&ptr == this)
                    return *this;
                this->m_ptr = ptr.m_ptr;
                return *this;
            }
            
            inline T* operator->(){
                return m_ptr;
            }
            
            ~nullptr_initializer() = default;
    };
}

#endif //BLT_TESTS_MEMORY_H

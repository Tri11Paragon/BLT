/*
 * Created by Brett on 08/02/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_TESTS_MEMORY_H
#define BLT_TESTS_MEMORY_H

#include <initializer_list>
#include <iterator>

namespace blt {
    
    template<typename V>
    struct ptr_iterator {
        public:
            using iterator_category = std::random_access_iterator_tag;
            using difference_type   = std::ptrdiff_t;
            using value_type        = V;
            using pointer           = value_type*;
            using reference         = value_type&;
            
            explicit ptr_iterator(V* v): _v(v) {}
            
            reference operator*() const { return *_v; }
            pointer operator->() { return _v; }
            ptr_iterator& operator++() {
                _v++;
                return *this;
            }
            ptr_iterator& operator--() {
                _v--;
                return *this;
            }
            ptr_iterator operator++(int){
                auto tmp = *this;
                ++(*this);
                return tmp;
            }
            ptr_iterator operator--(int){
                auto tmp = *this;
                --(*this);
                return tmp;
            }
            friend bool operator==(const ptr_iterator& a, const ptr_iterator& b) {
                return a._v == b._v;
            }
            friend bool operator!=(const ptr_iterator& a, const ptr_iterator& b) {
                return a._v != b._v;
            }
        
        private:
            V* _v;
    };
    
/**
     * Creates an encapsulation of a T array which will be automatically deleted when this object goes out of scope.
     * This is a simple buffer meant to be used only inside of a function and not moved around, with a few minor exceptions.
     * The internal buffer is allocated on the heap.
     * The operator * has been overloaded to return the internal buffer.
     * @tparam T type that is stored in buffer eg char
     */
    template<typename T>
    struct scoped_buffer {
        private:
            T* _buffer;
            size_t _size;
        public:
            explicit scoped_buffer(size_t size): _size(size) {
                _buffer = new T[size];
            }
            
            scoped_buffer(const scoped_buffer& copy) = delete;
            
            scoped_buffer(scoped_buffer&& move) noexcept {
                _buffer = move._buffer;
                _size = move.size();
                move._buffer = nullptr;
            }
            
            scoped_buffer operator=(scoped_buffer& copyAssignment) = delete;
            
            scoped_buffer& operator=(scoped_buffer&& moveAssignment) noexcept {
                _buffer = moveAssignment._buffer;
                _size = moveAssignment.size();
                moveAssignment._buffer = nullptr;
                
                return *this;
            }
            
            inline T& operator[](unsigned long index) {
                return _buffer[index];
            }
            
            inline const T& operator[](unsigned long index) const {
                return _buffer[index];
            }
            
            inline T* operator*(){
                return _buffer;
            }
            
            [[nodiscard]] inline size_t size() const {
                return _size;
            }
            
            inline T* ptr(){
                return _buffer;
            }
            
            ptr_iterator<T> begin(){
                return ptr_iterator{_buffer};
            }
            
            ptr_iterator<T> end(){
                return ptr_iterator{&_buffer[_size]};
            }
            
            ~scoped_buffer() {
                delete[] _buffer;
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
    
    /**
     * Creates a hash-map like association between an enum key and any arbitrary value.
     * The storage is backed by a contiguous array for faster access.
     * @tparam K enum value
     * @tparam V associated value
     */
    template<typename K, typename V>
    class enum_storage {
        private:
            V* _values;
            size_t _size = 0;
        public:
            enum_storage(std::initializer_list<std::pair<K, V>> init){
                for (auto& i : init)
                    _size = std::max((size_t)i.first, _size);
                _values = new V[_size];
                for (auto& v : init)
                    _values[(size_t)v.first] = v.second;
            }
            
            inline V& operator[](size_t index){
                return _values[index];
            }
            
            inline const V& operator[](size_t index) const {
                return _values[index];
            }
            
            [[nodiscard]] inline size_t size() const {
                return _size;
            }
            
            ptr_iterator<V> begin(){
                return ptr_iterator{_values};
            }
            
            ptr_iterator<V> end(){
                return ptr_iterator{&_values[_size]};
            }
            
            ~enum_storage(){
                delete[] _values;
            }
    };
}

#endif //BLT_TESTS_MEMORY_H

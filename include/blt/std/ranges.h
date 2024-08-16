#pragma once
/*
 * Created by Brett on 06/02/24.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_RANGES_H
#define BLT_RANGES_H

#include <blt/std/types.h>
#include <blt/std/meta.h>
#include <type_traits>
#include <iterator>
#include <memory>
#include <utility>
#include <limits>

namespace blt
{
    namespace itr
    {
        
        template<typename Begin, typename End>
        class itr_container
        {
            public:
                itr_container(Begin&& begin, End&& end): begin_(std::forward<Begin>(begin)), end_(std::forward<End>(end))
                {}
                
                Begin begin()
                {
                    return begin_;
                }
                
                End end()
                {
                    return end_;
                }
            
            private:
                Begin begin_;
                End end_;
        };
        
        template<typename TYPE_ITR>
        class iterator_base
        {
            public:
                using value_type = blt::meta::value_type_t<TYPE_ITR, std::remove_pointer_t<TYPE_ITR>>;
                using difference_type = blt::meta::difference_t<TYPE_ITR, std::ptrdiff_t>;
                using pointer = blt::meta::pointer_t<TYPE_ITR, TYPE_ITR>;
                using reference = blt::meta::reference_t<TYPE_ITR, std::remove_pointer_t<TYPE_ITR>&>;
                using const_reference = blt::meta::const_reference_t<TYPE_ITR, const std::remove_pointer_t<TYPE_ITR>&>;
        };
        
        template<typename TYPE_ITR>
        class iterator
        {
            public:
                using iterator_category = std::input_iterator_tag;
                using value_type = typename iterator_base<TYPE_ITR>::value_type;
                using difference_type = typename iterator_base<TYPE_ITR>::difference_type;
                using pointer = typename iterator_base<TYPE_ITR>::pointer;
                using reference = typename iterator_base<TYPE_ITR>::reference;
                using const_reference = typename iterator_base<TYPE_ITR>::const_reference;
            private:
                blt::size_t index;
                TYPE_ITR current;
            public:
                explicit iterator(TYPE_ITR current, blt::size_t index): index(index), current(std::move(current))
                {}
                
                auto iter()
                {
                    return current;
                }
                
                iterator& operator++()
                {
                    ++index;
                    ++current;
                    return *this;
                }
                
                bool operator==(iterator other) const
                {
                    return current == other.current;
                }
                
                bool operator!=(iterator other) const
                {
                    return current != other.current;
                }
                
                std::pair<blt::size_t, const_reference> operator*() const
                {
                    return {index, *current};
                };
                
                std::pair<blt::size_t, reference> operator*()
                {
                    return {index, *current};
                };
        };
        
        template<typename TYPE_ITR>
        class reverse_iterator
        {
            public:
                using iterator_category = std::input_iterator_tag;
                using value_type = typename iterator_base<TYPE_ITR>::value_type;
                using difference_type = typename iterator_base<TYPE_ITR>::difference_type;
                using pointer = typename iterator_base<TYPE_ITR>::pointer;
                using reference = typename iterator_base<TYPE_ITR>::reference;
                using const_reference = typename iterator_base<TYPE_ITR>::const_reference;
            private:
                blt::size_t index;
                TYPE_ITR current;
            public:
                explicit reverse_iterator(TYPE_ITR current, blt::size_t index): index(index), current(std::move(current))
                {}
                
                auto iter()
                {
                    return current;
                }
                
                reverse_iterator& operator++()
                {
                    --index;
                    --current;
                    return *this;
                }
                
                bool operator==(reverse_iterator other) const
                {
                    return current == other.current;
                }
                
                bool operator!=(reverse_iterator other) const
                {
                    return current != other.current;
                }
                
                std::pair<blt::size_t, const_reference> operator*() const
                {
                    return {index, *current};
                };
                
                std::pair<blt::size_t, reference> operator*()
                {
                    return {index, *current};
                };
        };
    }
    
    template<typename TYPE_ITR, template<typename> typename iterator = itr::iterator>
    class enumerator
    {
        public:
            explicit enumerator(TYPE_ITR begin, TYPE_ITR end): begin_(std::move(begin), 0), end_(std::move(end), 0)
            {}
            
            explicit enumerator(TYPE_ITR begin, TYPE_ITR end, blt::size_t i_begin, blt::size_t i_end):
                    begin_(std::move(begin), i_begin), end_(std::move(end), i_end)
            {}
            
            itr::iterator<TYPE_ITR> begin()
            {
                return begin_;
            }
            
            itr::iterator<TYPE_ITR> end()
            {
                return end_;
            }
        
        private:
            iterator<TYPE_ITR> begin_;
            iterator<TYPE_ITR> end_;
    };
    
    template<typename T>
    static inline auto enumerate(const T& container)
    {
        return enumerator{container.begin(), container.end()};
    }
    
    template<typename T, blt::size_t size>
    static inline auto enumerate(const T(& container)[size])
    {
        return enumerator{&container[0], &container[size]};
    }
    
    template<typename T, blt::size_t size>
    static inline auto enumerate(T(& container)[size])
    {
        return enumerator{&container[0], &container[size]};
    }
    
    template<typename T>
    static inline auto enumerate(T& container)
    {
        return enumerator{container.begin(), container.end()};
    }
    
    template<typename T>
    static inline auto enumerate(T&& container)
    {
        return enumerator{container.begin(), container.end()};
    }
    
    template<typename Begin, typename End>
    static inline auto iterate(Begin&& begin, End&& end)
    {
        return itr::itr_container<Begin, End>{std::forward<Begin>(begin), std::forward<End>(end)};
    }
    
    template<typename Begin, typename End>
    static inline auto reverse_iterate(Begin&& begin, End&& end)
    {
        return itr::itr_container{std::reverse_iterator(std::forward<Begin>(end)), std::reverse_iterator(std::forward<End>(begin))};
    }
    
    template<typename T>
    struct range
    {
        public:
            struct range_itr
            {
                public:
                    using iterator_category = std::bidirectional_iterator_tag;
                    using difference_type = T;
                    using value_type = T;
                    using pointer = T*;
                    using reference = T&;
                private:
                    T current;
                    bool forward;
                public:
                    
                    explicit range_itr(T current, bool forward): current(current), forward(forward)
                    {}
                    
                    value_type operator*() const
                    { return current; }
                    
                    value_type operator->()
                    { return current; }
                    
                    range_itr& operator++()
                    {
                        if (forward)
                            current++;
                        else
                            current--;
                        return *this;
                    }
                    
                    range_itr& operator--()
                    {
                        if (forward)
                            current--;
                        else
                            current++;
                        return *this;
                    }
                    
                    friend bool operator==(const range_itr& a, const range_itr& b)
                    {
                        return a.current == b.current;
                    }
                    
                    friend bool operator!=(const range_itr& a, const range_itr& b)
                    {
                        return a.current != b.current;
                    }
            };
        
        private:
            T _begin;
            T _end;
            T offset = 0;
        public:
            range(T begin, T end): _begin(begin), _end(end), offset(end < begin ? 1 : 0)
            {}
            
            range_itr begin()
            {
                return range_itr(_begin - offset, offset == 0);
            }
            
            range_itr end()
            {
                // not sure if i like this
                return range_itr(_end - offset, offset == 0);
            }
    };
    
    template<typename I>
    class itr_offset
    {
        private:
            I begin_;
            I end_;
        public:
            template<typename T>
            itr_offset(I begin, I end, T offset): begin_(begin), end_(end)
            {
                for (T t = 0; t < offset; t++)
                    ++begin_;
            }
            
            template<typename C, typename T>
            itr_offset(C& container, T offset): begin_(container.begin()), end_(container.end())
            {
                for (T t = 0; t < offset; t++)
                    ++begin_;
            }
            
            auto begin()
            {
                return begin_;
            }
            
            auto end()
            {
                return end_;
            }
    };
    
    template<typename C, typename T>
    itr_offset(C, T) -> itr_offset<typename C::iterator>;
    
    inline constexpr std::size_t dynamic_extent = std::numeric_limits<std::size_t>::max();
    
    template<typename T, std::size_t extent = dynamic_extent>
    class span;
    
    // https://codereview.stackexchange.com/questions/217814/c17-span-implementation
    namespace span_detail
    {
        // detect specializations of span
        template<class T>
        struct is_span : std::false_type
        {
        };
        
        template<class T, std::size_t N>
        struct is_span<span<T, N>> : std::true_type
        {
        };
        
        template<class T>
        inline constexpr bool is_span_v = is_span<T>::value;
        
        // detect specializations of std::array
        template<class T>
        struct is_array : std::false_type
        {
        };
        
        template<class T, std::size_t N>
        struct is_array<std::array<T, N>> : std::true_type
        {
        };
        
        template<class T>
        inline constexpr bool is_array_v = is_array<T>::value;
        
        // detect container
        template<class C, class = void>
        struct is_cont : std::false_type
        {
        };
        
        template<class C>
        struct is_cont<C,
                std::void_t<
                        std::enable_if_t<!is_span_v<C>>,
                        std::enable_if_t<!is_array_v<C>>,
                        std::enable_if_t<!std::is_array_v<C>>,
                        decltype(data(std::declval<C>())),
                        decltype(size(std::declval<C>()))
                >> : std::true_type
        {
        };
        
        template<class C>
        inline constexpr bool is_cont_v = is_cont<C>::value;
    }
    
    template<typename T, std::size_t extent>
    class span
    {
        public:
            using element_type = T;
            using value_type = std::remove_cv_t<T>;
            using size_type = blt::size_t;
            using difference_type = std::ptrdiff_t;
            using pointer = T*;
            using const_pointer = const T*;
            using reference = T&;
            using const_reference = const T&;
            using iterator = T*;
            using const_iterator = const T*;
            using reverse_iterator = std::reverse_iterator<iterator>;
            using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        private:
            size_type size_;
            pointer data_;
        public:
            constexpr span() noexcept: size_(0), data_(nullptr)
            {}
            
            constexpr span(T* data, size_type count): size_(count), data_(data)
            {}
            
            template<class It, std::size_t SIZE, typename std::enable_if_t<extent != dynamic_extent && SIZE == extent, bool> = true>
            constexpr explicit span(It first, size_type count): size_(count), data_(&*first)
            {}
            
            template<class It, std::size_t SIZE, typename std::enable_if_t<extent == dynamic_extent && SIZE == extent, bool> = true>
            constexpr span(It first, size_type count): size_(count), data_(&*first)
            {}
            
            template<class It, class End, std::size_t SIZE, typename std::enable_if_t<extent != dynamic_extent && SIZE == extent, bool> = true>
            constexpr explicit span(It first, End last): size_(&*last - &*first), data_(&*first)
            {}
            
            template<class It, class End, std::size_t SIZE, typename std::enable_if_t<extent == dynamic_extent && SIZE == extent, bool> = true>
            constexpr span(It first, End last): size_(&*last - &*first), data_(&*first)
            {}
            
            template<std::size_t N, typename std::enable_if_t<(N == dynamic_extent || N == extent) &&
                                                              (std::is_convertible_v<std::remove_pointer_t<decltype(
                                                              std::data(std::declval<T(&)[N]>()))>(*)[], T(*)[]>), bool> = true>
            constexpr span(element_type (& arr)[N]) noexcept: size_{N}, data_{arr} // NOLINT
            {}
            
            template<class U, std::size_t N, typename std::enable_if_t<(N == dynamic_extent || N == extent) &&
                                                                       (std::is_convertible_v<std::remove_pointer_t<decltype(
                                                                       std::data(std::declval<T(&)[N]>()))>(*)[], T(*)[]>), bool> = true>
            constexpr span(std::array<U, N>& arr) noexcept: size_(N), data_{arr.data()} // NOLINT
            {}
            
            template<class U, std::size_t N, typename std::enable_if_t<(N == dynamic_extent || N == extent) &&
                                                                       (std::is_convertible_v<std::remove_pointer_t<decltype(
                                                                       std::data(std::declval<T(&)[N]>()))>(*)[], T(*)[]>), bool> = true>
            constexpr span(const std::array<U, N>& arr) noexcept: size_(N), data_{arr.data()} // NOLINT
            {}
            
            template<class R, class RCV = std::remove_cv_t<std::remove_reference_t<R>>, typename std::enable_if_t<
                    extent != dynamic_extent && span_detail::is_cont_v<RCV> &&
                    std::is_convertible_v<std::remove_pointer_t<decltype(std::data(std::declval<R>()))>(*)[], T(*)[]>, bool> = true>
            explicit constexpr span(R&& range): size_(std::size(range)), data_(std::data(range))
            {}
            
            template<class R, class RCV = std::remove_cv_t<std::remove_reference_t<R>>, typename std::enable_if_t<
                    extent == dynamic_extent && span_detail::is_cont_v<RCV> &&
                    std::is_convertible_v<std::remove_pointer_t<decltype(std::data(std::declval<R>()))>(*)[], T(*)[]>, bool> = true>
            constexpr span(R&& range): size_(std::size(range)), data_(std::data(range)) // NOLINT
            {}
            
            template<size_type SIZE, typename std::enable_if_t<
                    extent != dynamic_extent && SIZE == extent && std::is_const_v<element_type>, bool> = true>
            explicit constexpr span(std::initializer_list<value_type> il) noexcept: size_(il.size()), data_(&il.begin()) // NOLINT
            {}
            
            template<size_type SIZE, typename std::enable_if_t<
                    extent == dynamic_extent && SIZE == extent && std::is_const_v<element_type>, bool> = true>
            explicit span(std::initializer_list<value_type> il) noexcept: size_(il.size()), data_(&il.begin()) // NOLINT
            {}
            
            template<class U, std::size_t N, typename std::enable_if_t<
                    extent != dynamic_extent && N == dynamic_extent && std::is_convertible_v<U(*)[], T(*)[]>, bool> = true>
            explicit constexpr span(const span<U, N>& source) noexcept: size_{source.size()}, data_{source.data()}
            {}
            
            template<class U, std::size_t N, typename std::enable_if_t<
                    !(extent != dynamic_extent && N == dynamic_extent) && std::is_convertible_v<U(*)[], T(*)[]>, bool> = true>
            constexpr span(const span<U, N>& source) noexcept: size_{source.size()}, data_{source.data()} // NOLINT
            {}
            
            constexpr span& operator=(const span& copy)
            {
                size_ = copy.size();
                data_ = copy.data();
                return *this;
            }
            
            constexpr span(const span& other) noexcept = default;
            
            constexpr iterator begin() const noexcept
            {
                return data();
            }
            
            constexpr iterator end() const noexcept
            {
                return data() + size();
            }
            
            constexpr const_iterator cbegin() const noexcept
            {
                return data();
            }
            
            constexpr const_iterator cend() const noexcept
            {
                return data() + size();
            }
            
            constexpr reverse_iterator rbegin() const noexcept
            {
                return reverse_iterator{end()};
            }
            
            constexpr reverse_iterator rend() const noexcept
            {
                return reverse_iterator{begin()};
            }
            
            constexpr const_reverse_iterator crbegin() const noexcept
            {
                return reverse_iterator{cend()};
            }
            
            constexpr const_reverse_iterator crend() const noexcept
            {
                return reverse_iterator{cbegin()};
            }
            
            friend constexpr iterator begin(span s) noexcept
            {
                return s.begin();
            }
            
            friend constexpr iterator end(span s) noexcept
            {
                return s.end();
            }
            
            [[nodiscard]] constexpr size_type size() const noexcept
            {
                return size_;
            }
            
            [[nodiscard]] constexpr size_type size_bytes() const noexcept
            {
                return size() * sizeof(T);
            }
            
            [[nodiscard]] constexpr bool empty() const noexcept
            {
                return size() == 0;
            }
            
            constexpr reference operator[](size_type idx) const
            {
                return *(data() + idx);
            }
            
            constexpr reference front() const
            {
                return *data();
            }
            
            constexpr reference back() const
            {
                return *(data() + (size() - 1));
            }
            
            constexpr pointer data() const noexcept
            {
                return data_;
            }
            
            constexpr span<T, dynamic_extent> first(size_type cnt) const
            {
                return {data(), cnt};
            }
            
            constexpr span<T, dynamic_extent> last(size_type cnt) const
            {
                return {data() + (size() - cnt), cnt};
            }
            
            constexpr span<T, dynamic_extent> subspan(size_type off, size_type cnt = dynamic_extent) const
            {
                return {data() + off, cnt == dynamic_extent ? size() - off : cnt};
            }
        
    };
    
    template<class T, std::size_t N>
    span(T (&)[N]) -> span<T, N>;
    
    template<class T, std::size_t N>
    span(std::array<T, N>&) -> span<T, N>;
    
    template<class T, std::size_t N>
    span(const std::array<T, N>&) -> span<const T, N>;
    
    template<class Cont>
    span(Cont&) -> span<typename Cont::value_type>;
    
    template<class Cont>
    span(const Cont&) -> span<const typename Cont::value_type>;
}

#endif //BLT_RANGES_H

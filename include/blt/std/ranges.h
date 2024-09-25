#pragma once
/*
 * Created by Brett on 06/02/24.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_RANGES_H
#define BLT_RANGES_H

#include <blt/std/types.h>
#include <blt/meta/meta.h>
#include <blt/meta/iterator.h>
#include <type_traits>
#include <iterator>
#include <memory>
#include <utility>
#include <limits>

namespace blt
{
    template<typename Iter, typename = std::void_t<>>
    class enumerate_wrapper;
    
    template<typename Iter, typename = std::void_t<>>
    class enumerator;
    
    template<typename Iter, typename = std::void_t<>>
    class enumerator_rev;
    
    template<typename T>
    struct enumerate_item
    {
        blt::size_t index;
        T value;
    };
    
    template<typename Iter>
    class enumerate_iterator_base
    {
        public:
            explicit enumerate_iterator_base(Iter iter, blt::size_t place = 0): iter(std::move(iter)), index(place)
            {}
            
            enumerate_item<blt::meta::deref_return_t<Iter>> operator*() const
            {
                return {index, *this->iter};
            }
            
            friend bool operator==(const enumerate_iterator_base& a, const enumerate_iterator_base& b)
            {
                return a.iter == b.iter;
            }
            
            friend bool operator!=(const enumerate_iterator_base& a, const enumerate_iterator_base& b)
            {
                return a.iter != b.iter;
            }
            
            auto base() const
            {
                return iter;
            }
            
            auto get_index() const
            {
                return index;
            }
        
        protected:
            Iter iter;
            blt::size_t index;
    };
    
    template<typename Iter>
    class enumerate_forward_iterator : public enumerate_iterator_base<Iter>
    {
        public:
            using enumerate_iterator_base<Iter>::enumerate_iterator_base;
            
            enumerate_forward_iterator& operator++()
            {
                ++this->iter;
                ++this->index;
                return *this;
            }
            
            enumerate_forward_iterator operator++(int)
            {
                auto tmp = *this;
                ++*this;
                return tmp;
            }
    };
    
    template<typename Iter>
    class enumerate_bidirectional_iterator : public enumerate_forward_iterator<Iter>
    {
        public:
            using enumerate_forward_iterator<Iter>::enumerate_forward_iterator;
            
            enumerate_bidirectional_iterator& operator--()
            {
                --this->iter;
                --this->index;
                return *this;
            }
            
            enumerate_bidirectional_iterator operator--(int)
            {
                auto tmp = *this;
                --*this;
                return tmp;
            }
    };
    
    template<typename Iter>
    class enumerate_wrapper<Iter, std::enable_if_t<blt::meta::is_forward_iterator_v<Iter>, std::void_t<std::forward_iterator_tag>>>
            : public enumerate_forward_iterator<Iter>
    {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = enumerate_item<blt::meta::deref_return_t<Iter>>;
            using difference_type = typename std::iterator_traits<Iter>::difference_type;
            using pointer = value_type;
            using reference = value_type;
            using iterator_type = Iter;
            
            using enumerate_forward_iterator<Iter>::enumerate_forward_iterator;
    };
    
    template<typename Iter>
    class enumerate_wrapper<Iter, std::enable_if_t<blt::meta::is_bidirectional_or_better_v<Iter>, std::void_t<std::bidirectional_iterator_tag>>>
            : public enumerate_bidirectional_iterator<Iter>
    {
        public:
            using iterator_category = typename std::iterator_traits<Iter>::iterator_category;
            using value_type = enumerate_item<blt::meta::deref_return_t<Iter>>;
            using difference_type = typename std::iterator_traits<Iter>::difference_type;
            using pointer = value_type;
            using reference = value_type;
            using iterator_type = Iter;
            
            using enumerate_bidirectional_iterator<Iter>::enumerate_bidirectional_iterator;
    };
    
    template<typename Iter, typename IterWrapper>
    class enumerator_base
    {
        public:
            explicit enumerator_base(Iter begin, Iter end): begin_(std::move(begin)), end_(std::move(end))
            {}
            
            explicit enumerator_base(IterWrapper begin, IterWrapper end): begin_(std::move(begin)), end_(std::move(end))
            {}
            
            auto begin()
            {
                return begin_;
            }
            
            auto end()
            {
                return end_;
            }
        
        protected:
            IterWrapper begin_;
            IterWrapper end_;
    };
    
    template<typename Iter, typename IterWrapper>
    class enumerator_reversible : public enumerator_base<Iter, IterWrapper>
    {
        public:
            explicit enumerator_reversible(Iter begin, Iter end, blt::size_t container_size):
                    enumerator_base<Iter, IterWrapper>{IterWrapper{enumerate_wrapper<Iter>{std::move(begin), 0}},
                                                       IterWrapper{enumerate_wrapper<Iter>{std::move(end), container_size}}},
                    container_size(container_size)
            {}
            
            explicit enumerator_reversible(Iter begin, Iter end, blt::size_t begin_index, blt::size_t end_index, blt::size_t container_size):
                    enumerator_base<Iter, IterWrapper>(IterWrapper{enumerate_wrapper<Iter>{std::move(begin), begin_index}},
                                                       IterWrapper{enumerate_wrapper<Iter>{std::move(end), end_index}}),
                    container_size(container_size)
            {}
            
            auto rev() const
            {
                return enumerator_rev<Iter>{this->end_.base(), this->begin_.base(), this->container_size, this->begin_.get_index(),
                                            this->container_size};
            }
        
        protected:
            blt::size_t container_size;
    };
    
    template<typename Iter, typename IterWrapper>
    class enumerator_reversible_rev : public enumerator_reversible<Iter, IterWrapper>
    {
        public:
            using enumerator_reversible<Iter, IterWrapper>::enumerator_reversible;
            
            auto rev() const
            {
                BLT_TRACE(this->end_.base().get_index());
                BLT_TRACE(this->begin_.base().get_index());
                return enumerator<Iter>{this->end_.base().base(), this->begin_.base().base(), this->end_.base().get_index(),
                                        this->begin_.base().get_index(), this->container_size};
            }
    };
    
    template<typename Iter>
    class enumerator<Iter, std::enable_if_t<blt::meta::is_forward_iterator_v<Iter>, std::void_t<std::forward_iterator_tag>>>
            : public enumerator_base<Iter, enumerate_wrapper<Iter>>
    {
        public:
            using enumerator_base<Iter, enumerate_wrapper<Iter>>::enumerator_base;
    };
    
    template<typename Iter>
    class enumerator<Iter, std::enable_if_t<blt::meta::is_bidirectional_iterator_v<Iter>, std::void_t<std::bidirectional_iterator_tag>>>
            : public enumerator_reversible<Iter, enumerate_wrapper<Iter>>
    {
        public:
            using enumerator_reversible<Iter, enumerate_wrapper<Iter>>::enumerator_reversible;
    };
    
    template<typename Iter>
    class enumerator<Iter, std::enable_if_t<blt::meta::is_random_access_iterator_v<Iter>, std::void_t<std::random_access_iterator_tag>>>
            : public enumerator_reversible<Iter, enumerate_wrapper<Iter>>
    {
        public:
            using enumerator_reversible<Iter, enumerate_wrapper<Iter>>::enumerator_reversible;
            
            auto skip(blt::size_t offset)
            {
                return enumerator<Iter>{this->begin_.base() + offset, this->end_.base(), this->begin_.get_index() + offset, this->container_size,
                                        this->container_size};
            }
    };
    
    template<typename Iter>
    class enumerator_rev<Iter, std::enable_if_t<blt::meta::is_bidirectional_iterator_v<Iter>, std::void_t<std::bidirectional_iterator_tag>>>
            : public enumerator_reversible_rev<Iter, std::reverse_iterator<enumerate_wrapper<Iter>>>
    {
        public:
            using enumerator_reversible_rev<Iter, std::reverse_iterator<enumerate_wrapper<Iter>>>::enumerator_reversible_rev;
    };
    
    template<typename Iter>
    class enumerator_rev<Iter, std::enable_if_t<blt::meta::is_random_access_iterator_v<Iter>, std::void_t<std::random_access_iterator_tag>>>
            : public enumerator_reversible_rev<Iter, std::reverse_iterator<enumerate_wrapper<Iter>>>
    {
        public:
            using enumerator_reversible_rev<Iter, std::reverse_iterator<enumerate_wrapper<Iter>>>::enumerator_reversible_rev;
            
            auto skip(blt::size_t offset)
            {
                return enumerator_rev<Iter>{this->begin_.base().base() - offset, this->end_.base().base(), this->begin_.base().get_index() - offset,
                                            this->container_size, this->container_size};
            }
    };
    
    template<typename Iter>
    enumerator(Iter, Iter) -> enumerator<Iter>;
    
    template<typename Iter>
    enumerator(Iter, Iter, blt::size_t) -> enumerator<Iter>;
    
    template<typename Iter>
    enumerator(Iter, Iter, blt::size_t, blt::size_t) -> enumerator<Iter>;
    
    template<typename T>
    static inline auto enumerate(const T& container)
    {
        return enumerator{container.begin(), container.end(), container.size()};
    }
    
    template<typename T, blt::size_t size>
    static inline auto enumerate(const T(& container)[size])
    {
        return enumerator{&container[0], &container[size], size};
    }
    
    template<typename T, blt::size_t size>
    static inline auto enumerate(T(& container)[size])
    {
        return enumerator{&container[0], &container[size], size};
    }
    
    template<typename T>
    static inline auto enumerate(T& container)
    {
        return enumerator{container.begin(), container.end(), container.size()};
    }
    
    template<typename T>
    static inline auto enumerate(T&& container)
    {
        return enumerator{container.begin(), container.end(), container.size()};
    }
    
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
        
        // TODO: cleanup! all of this! add support for reversing
        template<typename C1_TYPE, typename C2_TYPE>
        class pair_iterator
        {
            public:
                using c1_ref = blt::meta::deref_return_t<C1_TYPE>;
                using c2_ref = blt::meta::deref_return_t<C2_TYPE>;
                
                using iterator_category = std::forward_iterator_tag;
                using value_type = std::pair<c1_ref, c2_ref>;
                using difference_type = blt::ptrdiff_t;
                using pointer = void*;
                using reference = value_type&;
                using const_reference = const value_type&;
                
                explicit pair_iterator(C1_TYPE c1, C2_TYPE c2): current_c1_iter(c1), current_c2_iter(c2)
                {}
                
                pair_iterator& operator++()
                {
                    ++current_c1_iter;
                    ++current_c2_iter;
                    return *this;
                }
                
                bool operator==(pair_iterator other) const
                {
                    return current_c1_iter == other.current_c1_iter && current_c2_iter == other.current_c2_iter;
                }
                
                bool operator!=(pair_iterator other) const
                {
                    return current_c1_iter != other.current_c1_iter || current_c2_iter != other.current_c2_iter;
                }
                
                value_type operator*() const
                {
                    return {*current_c1_iter, *current_c2_iter};
                };
                
                value_type operator*()
                {
                    return {*current_c1_iter, *current_c2_iter};
                };
            
            private:
                C1_TYPE current_c1_iter;
                C2_TYPE current_c2_iter;
        };
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
    
    template<typename C1_ITER, typename C2_ITER, template<typename, typename> typename iterator = itr::pair_iterator>
    class pair_enumerator
    {
        public:
            explicit pair_enumerator(C1_ITER c1_begin, C1_ITER c1_end, C2_ITER c2_begin, C2_ITER c2_end):
                    begin_(std::move(c1_begin), std::move(c2_begin)), end_(std::move(c1_end), std::move(c2_end))
            {
                auto size_c1 = c1_end - c1_begin;
                auto size_c2 = c2_end - c2_begin;
                if (size_c1 != size_c2)
                    throw std::runtime_error("Iterator sizes don't match!");
            }
            
            iterator<C1_ITER, C2_ITER> begin()
            {
                return begin_;
            }
            
            iterator<C1_ITER, C2_ITER> end()
            {
                return end_;
            }
        
        private:
            iterator<C1_ITER, C2_ITER> begin_;
            iterator<C1_ITER, C2_ITER> end_;
    };
    
    template<typename T, typename G>
    static inline auto in_pairs(const T& container1, const G& container2)
    {
        return pair_enumerator{container1.begin(), container1.end(), container2.begin(), container2.end()};
    }
    
    template<typename T, typename G>
    static inline auto in_pairs(T& container1, G& container2)
    {
        return pair_enumerator{container1.begin(), container1.end(), container2.begin(), container2.end()};
    }
    
    template<typename T, typename G, blt::size_t size>
    static inline auto in_pairs(const T(& container1)[size], const G(& container2)[size])
    {
        return pair_enumerator{&container1[0], &container1[size], &container2[0], &container2[size]};
    }
    
    template<typename T, typename G, blt::size_t size>
    static inline auto in_pairs(T(& container1)[size], G(& container2)[size])
    {
        return pair_enumerator{&container1[0], &container1[size], &container2[0], &container2[size]};
    }
    
    template<typename T, typename G>
    static inline auto in_pairs(T&& container1, G&& container2)
    {
        return pair_enumerator{container1.begin(), container1.end(), container2.begin(), container2.end()};
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

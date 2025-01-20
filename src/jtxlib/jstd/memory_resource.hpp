#pragma once
#include "jtxlib.hpp"
#include "jtxlib/math/vecmath.hpp"

#include <jtxlib/util/assert.hpp>

#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <utility>

namespace jtx {
namespace pmr {

#pragma region Memory Resource
/**
 * Implementation of the C++17 polymorphic memory resource interface.
 *
 * Main diff from std implementation is some of the return behavior
 *
 * References:
 *  - https://github.com/gcc-mirror/gcc/blob/0121b852c85db91babdb346f277ca6220eb7df86/libstdc%2B%2B-v3/include/bits/memory_resource.h#L61
 *  - https://github.com/mmp/pbrt-v4/blob/39e01e61f8de07b99859df04b271a02a53d9aeb2/src/pbrt/util/pstd.h#L483
 */
class memory_resource {
    static constexpr size_t max_align = alignof(std::max_align_t);
public:
    memory_resource() = default;
    memory_resource(const memory_resource &) = delete;
    virtual ~memory_resource() = default;

    memory_resource &operator=(const memory_resource &) = default;

    [[nodiscard]] void *allocate(size_t bytes, size_t alignment = max_align) {
        if (bytes == 0) return nullptr;
        return do_allocate(bytes, alignment);
    }

    void deallocate(void *p, size_t bytes, size_t alignment = max_align) {
        if (p == nullptr) return;
        do_deallocate(p, bytes, alignment);
    }

    [[nodiscard]] bool is_equal(const memory_resource &other) const noexcept {
        return do_is_equal(other);
    }

private:
    virtual void *do_allocate(size_t bytes, size_t alignment) = 0;
    virtual void do_deallocate(void *p, size_t bytes, size_t alignment) = 0;
    [[nodiscard]] virtual bool do_is_equal(const memory_resource &other) const noexcept = 0;
};

[[nodiscard]] inline bool operator==(const memory_resource &a, const memory_resource &b) noexcept {
    return &a == &b || a.is_equal(b);
}

[[nodiscard]] inline bool operator!=(const memory_resource &a, const memory_resource &b) noexcept {
    return !(a == b);
}
#pragma endregion Memory Resource

#pragma region Global Memory Resources
/**
 * Global memory resources
 */
[[nodiscard]] memory_resource *new_delete_resource() noexcept;

[[nodiscard]] memory_resource *null_memory_resource() noexcept;

[[nodiscard]] memory_resource *set_default_resource(memory_resource *r) noexcept;

[[nodiscard]] memory_resource *get_default_resource() noexcept;
#pragma endregion Global Memory Resources

#pragma region Polymorphic Allocator
/**
 * Implementation of the C++17 polymorphic allocator interface.
 *
 * References:
 *  -
 * https://github.com/gcc-mirror/gcc/blob/0121b852c85db91babdb346f277ca6220eb7df86/libstdc%2B%2B-v3/include/bits/memory_resource.h#L61
 *  -
 * https://github.com/mmp/pbrt-v4/blob/39e01e61f8de07b99859df04b271a02a53d9aeb2/src/pbrt/util/pstd.h#L483
 * @tparam Tp The type of the object to allocate.
 */
template<typename Tp>
class polymorphic_allocator {
public:
    using value_type = Tp;

    polymorphic_allocator() noexcept {
        m_resource = get_default_resource();
    }

    polymorphic_allocator(memory_resource *r) noexcept : m_resource(r) {}// NOLINT(*-explicit-constructor)

    polymorphic_allocator(polymorphic_allocator const &other) = default;

    template<typename U>
    // ReSharper disable once CppNonExplicitConvertingConstructor
    polymorphic_allocator(polymorphic_allocator<U> const &other) noexcept : m_resource(other.resource()) {}// NOLINT(*-explicit-constructor)

    polymorphic_allocator &operator=(const polymorphic_allocator &other) = delete;

    [[nodiscard]] Tp *allocate(size_t n) {
        return static_cast<Tp *>(m_resource->allocate(n * sizeof(Tp), alignof(Tp)));
    }

    void deallocate(Tp *p, size_t n) {
        m_resource->deallocate(p, n * sizeof(Tp), alignof(Tp));
    }

    [[nodiscard]] void *allocate_bytes(size_t n, size_t alignment = alignof(std::max_align_t)) {
        return m_resource->allocate(n, alignment);
    }

    void deallocate_bytes(void *p, size_t n, size_t alignment = alignof(std::max_align_t)) {
        m_resource->deallocate(p, n, alignment);
    }

    template<typename Up>
    [[nodiscard]] Up *allocate_object(size_t n = 1) {
        return static_cast<Up *>(allocate_bytes(n * sizeof(Up), alignof(Up)));
    }

    template<typename Up>
    void deallocate_object(Up *p, size_t n = 1) {
        deallocate_bytes(p, n);
    }

    template<typename Up, typename... Args>
    void construct(Up *p, Args &&...args) const {
        new (p) Up(std::forward<Args>(args)...);
    }

    template<class Up, class... Args>
    Up *new_object(Args &&...args) {
        Up *p = allocate_object<Up>();
        construct(p, std::forward<Args>(args)...);
        return p;
    }

    template<class Up>
    void delete_object(Up *p) {
        destroy(p);
        deallocate_object(p);
    }

    // This is not std, but PBRT does this for a small optimization in pmr::vector
    template<class Up>
    void destroy(Up *p) {
        p->~Up();
    }

    [[nodiscard]] memory_resource *resource() const noexcept { return m_resource; }

private:
    memory_resource *m_resource;
};

template<typename Tp1, typename Tp2>
[[nodiscard]] bool operator==(const polymorphic_allocator<Tp1> &a, const polymorphic_allocator<Tp2> &b) noexcept {
    return a.resource() == b.resource();
}

template<typename Tp1, typename Tp2>
[[nodiscard]] bool operator!=(const polymorphic_allocator<Tp1> &a, const polymorphic_allocator<Tp2> &b) noexcept {
    return !(a == b);
}
#pragma endregion Polymorphic Allocator

}

#pragma region PMR Vector

static const int PMR_VECTOR_EMPTY_RESERVE = 4;
static const int PMR_VECTOR_GROWTH_FACTOR = 2;

/**
 * Implementation of the C++17 polymorphic vector interface.
 * Note, a few methods are left unimplemented,
 *
 * References:
 *  - https://en.cppreference.com/w/cpp/container/vector
 *  - https://github.com/mmp/pbrt-v4/blob/39e01e61f8de07b99859df04b271a02a53d9aeb2/src/pbrt/util/pstd.h#L701
 *  - https://gcc.gnu.org/onlinedocs/gcc-4.6.3/libstdc++/api/a01115_source.html
 * @tparam Tp The type of the object to allocate.
 * @tparam Allocator The allocator to use.
 */
template<typename Tp, class Allocator = pmr::polymorphic_allocator<Tp>>
class vector {
public:
#pragma region Typedefs
    using value_type = Tp;
    using allocator_type = Allocator;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = Tp *;
    using const_pointer = const Tp *;
    using iterator = Tp *;
    using const_iterator = const Tp *;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
#pragma endregion Typedefs

#pragma region Constructors/destructors
    JTX_HOST
    explicit vector(const Allocator &alloc = {}) noexcept : alloc(alloc) {}

    JTX_HOST
    vector(size_type count, const Tp &value, const Allocator &alloc = {}) : alloc(alloc) {
        reserve(count);
        for (size_t i = 0; i < count; ++i) {
            this->alloc.template construct<Tp>(ptr + i, value);
        }
        numStored = count;
    }

    JTX_HOST
    explicit vector(size_type count, const Allocator &alloc = {}) : vector(count, Tp{}, alloc) {}

    JTX_HOST
    template<class InputIt>
    vector(InputIt first, InputIt last, const Allocator &alloc = {}) : alloc(alloc) {
        reserve(last - first);
        size_t i = 0;
        for (InputIt iter = first; iter != last; ++iter, ++i) {
            this->alloc.template construct<Tp>(ptr + i, *iter);
        }
        numStored = numAlloc;
    }

    JTX_HOST
    vector(vector &other) : alloc(other.alloc) {
        numStored = other.numStored;
        numAlloc = other.numAlloc;
        ptr = other.ptr;

        other.numStored = other.numAlloc = 0;
        other.ptr = nullptr;
    }

    JTX_HOST
    vector(vector &other, const Allocator &alloc) {
        if (alloc == other.alloc) {
            ptr = other.ptr;
            numAlloc = other.numAlloc;
            numStored = other.numStored;

            other.numStored = other.numAlloc = 0;
            other.ptr = nullptr;
        } else {
            reserve(other.size());
            for (size_t i = 0; i < other.size(); ++i) {
                alloc.template construct<Tp>(ptr + i, std::move(other[i]));
            }
            numStored = other.size();
        }
    }

    JTX_HOST
    vector(std::initializer_list<Tp> ilist, const Allocator &alloc = {}) : vector(ilist.begin(), ilist.end(), alloc) {}

    JTX_HOST
    ~vector() {
        clear();
        alloc.deallocate_object(ptr, numAlloc);
    }
#pragma endregion Constructors / destructors

    JTX_HOST
    void assign(size_type count, const Tp &value) {
        clear();
        reserve(count);
        for (size_t i = 0; i < count; ++i) {
            push_back(value);
        }
    }

    JTX_HOST
    template<class InputIt>
    void assign(InputIt first, InputIt last) {
        clear();
        reserve(last - first);
        for (auto iter = first; iter != last; ++iter) {
            push_back(*iter);
        }
    }

    JTX_HOST
    void assign(std::initializer_list<Tp> ilist) {
        assign(ilist.begin(), ilist.end());
    }

    JTX_HOST
    allocator_type get_allocator() const noexcept { return alloc; }

    JTX_HOST
    vector &operator=(const vector &other) {
        if (this == &other) return *this;
        clear();
        reserve(other.size());
        for (size_t i = 0; i < other.size(); ++i) {
            alloc.template construct<Tp>(ptr + i, other[i]);
        }
        numStored = other.size();
        return *this;
    }

    JTX_HOST
    vector &operator=(vector &&other) noexcept {
        if (this == &other) return *this;
        clear();

        if (alloc == other.alloc) {
            std::swap(ptr, other.ptr);
            std::swap(numAlloc, other.numAlloc);
            std::swap(numStored, other.numStored);
        } else {
            clear();
            reserve(other.size());
            for (size_t i = 0; i < other.size(); ++i) {
                alloc.template construct<Tp>(ptr + i, std::move(other[i]));
            }
            numStored = other.size();
        }
        return *this;
    }

    JTX_HOST
    vector &operator=(std::initializer_list<Tp> &init) {
        reserve(init.size());
        clear();
        iterator iter = begin();
        for (const auto &value: init) {
            *iter = value;
            ++iter;
        }
        return *this;
    }

#pragma region Element access
    JTX_HOSTDEV
    reference at(size_type pos) {
        ASSERT(pos < numStored);
        return ptr[pos];
    }

    JTX_HOSTDEV
    const_reference at(size_type pos) const {
        ASSERT(pos < numStored);
        return ptr[pos];
    }

    JTX_HOSTDEV
    reference operator[](size_type pos) {
        ASSERT(pos < numStored);
        return ptr[pos];
    }

    JTX_HOSTDEV
    const_reference operator[](size_type pos) const {
        ASSERT(pos < numStored);
        return ptr[pos];
    }

    JTX_HOSTDEV
    reference front() {
        ASSERT(numStored > 0);
        return ptr[0];
    }

    JTX_HOSTDEV
    const_reference front() const {
        ASSERT(numStored > 0);
        return ptr[0];
    }

    JTX_HOSTDEV
    reference back() {
        ASSERT(numStored > 0);
        return ptr[numStored - 1];
    }

    JTX_HOSTDEV
    const_reference back() const {
        ASSERT(numStored > 0);
        return ptr[numStored - 1];
    }

    JTX_HOSTDEV
    Tp *data() noexcept { return ptr; }

    JTX_HOSTDEV
    const Tp *data() const noexcept { return ptr; }
#pragma endregion Element access

#pragma region Iterators
    JTX_HOSTDEV
    iterator begin() { return ptr; }

    JTX_HOSTDEV
    const_iterator begin() const { return ptr; }

    JTX_HOSTDEV
    const_iterator cbegin() const { return ptr; }

    JTX_HOSTDEV
    const_iterator end() const { return ptr + numStored; }

    JTX_HOSTDEV
    iterator end() { return ptr + numStored; }

    JTX_HOSTDEV
    const_iterator cend() const { return ptr + numStored; }

    JTX_HOSTDEV
    iterator rbegin() { return reverse_iterator(end()); }

    JTX_HOSTDEV
    const_iterator rbegin() const { return const_reverse_iterator(end()); }

    JTX_HOSTDEV
    const_iterator crbegin() const { return const_reverse_iterator(end()); }

    JTX_HOSTDEV
    iterator rend() { return reverse_iterator(begin()); }

    JTX_HOSTDEV
    const_iterator rend() const { return const_reverse_iterator(begin()); }

    JTX_HOSTDEV
    const_iterator crend() const { return const_reverse_iterator(begin()); }
#pragma endregion Iterators

#pragma region Capacity
    [[nodiscard]] JTX_HOSTDEV bool empty() const { return numStored == 0; }

    [[nodiscard]] JTX_HOSTDEV
            size_t
            size() const { return numStored; }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    [[nodiscard]] JTX_HOSTDEV
            size_t
            max_size() const { return static_cast<size_t>(-1); }

    // Allocators cannot be used on the GPU
    JTX_HOST
    void reserve(size_t n) {
        if (numAlloc >= n) return;
        Tp *newPtr = alloc.template allocate_object<Tp>(n);
        for (int i = 0; i < numStored; ++i) {
            alloc.template construct<Tp>(newPtr + i, std::move(ptr[i]));
            alloc.destroy(begin() + i);
        }
        alloc.deallocate_object(ptr, numAlloc);
        numAlloc = n;
        ptr = newPtr;
    }

    [[nodiscard]] JTX_HOSTDEV
            size_t
            capacity() const { return numAlloc; }

#pragma endregion Capacity

#pragma region Modifiers
    JTX_HOST
    void clear() noexcept {
        for (int i = 0; i < numStored; ++i) {
            alloc.destroy(&ptr[i]);
        }
        numStored = 0;
    }

    JTX_HOST
    iterator insert(const_iterator pos, const Tp &value) {
        ASSERT(pos >= begin() && pos <= end());
        if (pos == end()) {
            push_back(value);
            return end() - 1;
        }
        if (numStored == numAlloc) {
            reserve(numAlloc == 0 ? PMR_VECTOR_EMPTY_RESERVE : numAlloc * PMR_VECTOR_GROWTH_FACTOR);
        }
        size_t index = pos - begin();
        for (size_t i = numStored; i > index; --i) {
            alloc.construct(ptr + i, std::move(ptr[i - 1]));
            alloc.destroy(ptr + i - 1);
        }
        alloc.construct(ptr + index, value);
        ++numStored;
        return begin() + index;
    }

    JTX_HOST
    iterator insert(const_iterator pos, Tp &&value) {
        ASSERT(pos >= begin() && pos <= end());
        if (pos == end()) {
            push_back(std::move(value));
            return end() - 1;
        }
        if (numStored == numAlloc) {
            reserve(numAlloc == 0 ? PMR_VECTOR_EMPTY_RESERVE : numAlloc * PMR_VECTOR_GROWTH_FACTOR);
        }
        size_t index = pos - begin();
        for (size_t i = numStored; i > index; --i) {
            alloc.construct(ptr + i, std::move(ptr[i - 1]));
            alloc.destroy(ptr + i - 1);
        }
        alloc.construct(ptr + index, std::move(value));
        ++numStored;
        return begin() + index;
    }

    JTX_HOST
    iterator insert(const_iterator pos, size_type count, const Tp &value) {
        ASSERT(pos >= begin() && pos <= end());
        if (count == 0) return pos;
        if (numStored == numAlloc) {
            reserve(numAlloc == 0 ? std::max(PMR_VECTOR_EMPTY_RESERVE, static_cast<int>(count)) : std::max(numAlloc * PMR_VECTOR_GROWTH_FACTOR, count));
        }
        size_t index = pos - begin();
        for (size_t i = numStored + count - 1; i > index + count - 1; --i) {
            alloc.construct(ptr + i, std::move(ptr[i - count]));
            alloc.destroy(ptr + i - count);
        }
        for (size_t i = index; i < index + count; ++i) {
            alloc.construct(ptr + i, value);
        }
        numStored += count;
        return ptr + index;
    }

    /**
     * Not implemented; will be implemented if needed
     */
    JTX_HOST
    template<class InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        ASSERT(false);
    }

    /**
     * Not implemented; will be implemented if needed
     */
    JTX_HOST
    iterator insert(const_iterator pos, std::initializer_list<Tp> ilist) {
        ASSERT(false);
    }

    JTX_HOST
    template<class... Args>
    iterator emplace(const_iterator pos, Args &&...args) {
        ASSERT(pos >= begin() && pos <= end());
        if (pos == end()) {
            emplace_back(std::forward<Args>(args)...);
            return end() - 1;
        }
        if (numStored == numAlloc) {
            reserve(numAlloc == 0 ? PMR_VECTOR_EMPTY_RESERVE : numAlloc * PMR_VECTOR_GROWTH_FACTOR);
        }
        size_t index = pos - begin();
        for (size_t i = numStored; i > index; --i) {
            alloc.construct(ptr + i, std::move(ptr[i - 1]));
            alloc.destroy(ptr + i - 1);
        }
        alloc.construct(ptr + index, std::forward<Args>(args)...);
        ++numStored;
        return begin() + index;
    }

    JTX_HOST
    iterator erase(const_iterator pos) {
        ASSERT(pos >= begin() && pos < end());
        size_t index = pos - begin();
        alloc.destroy(ptr + index);
        for (size_t i = index + 1; i < numStored; ++i) {
            alloc.construct(ptr + i - 1, std::move(ptr[i]));
            alloc.destroy(ptr + i);
        }
        --numStored;
        if (index == numStored) return end();
        return begin() + index;
    }

    /**
     * Not implemented; will be implemented if needed
     */
    JTX_HOST
    iterator erase(const_iterator first, const_iterator last) {
        ASSERT(false);
    }

    JTX_HOST
    void push_back(const Tp &value) {
        if (numStored == numAlloc) {
            reserve(numAlloc == 0 ? PMR_VECTOR_EMPTY_RESERVE : numAlloc * PMR_VECTOR_GROWTH_FACTOR);
        }
        alloc.construct(ptr + numStored, value);
        ++numStored;
    }

    JTX_HOST
    void push_back(Tp &&value) {
        if (numStored == numAlloc) {
            reserve(numAlloc == 0 ? PMR_VECTOR_EMPTY_RESERVE : numAlloc * PMR_VECTOR_GROWTH_FACTOR);
        }
        alloc.construct(ptr + numStored, std::move(value));
        ++numStored;
    }

    JTX_HOST
    template<class... Args>
    reference emplace_back(Args &&...args) {
        if (numStored == numAlloc) {
            reserve(numAlloc == 0 ? PMR_VECTOR_EMPTY_RESERVE : numAlloc * PMR_VECTOR_GROWTH_FACTOR);
        }
        alloc.construct(ptr + numStored, std::forward<Args>(args)...);
        return ptr[numStored++];
    }

    JTX_HOST
    void pop_back() {
        ASSERT(numStored > 0);
        alloc.destroy(ptr + numStored - 1);
        --numStored;
    }

    JTX_HOST
    void resize(size_type count) {
        if (count < size()) {
            for (size_t i = count; i < size(); ++i) {
                alloc.destroy(ptr + i);
            }
            if (count == 0) {
                alloc.deallocate_object(ptr, numAlloc);
                ptr = nullptr;
                numAlloc = 0;
            }
        } else if (count > size()) {
            reserve(count);
            for (size_t i = numStored; i < count; ++i) {
                alloc.construct(ptr + i);
            }
        }
        numStored = count;
    }

    JTX_HOST
    void resize(size_type count, const value_type &value) {
        if (count < size()) {
            for (size_t i = count; i < size(); ++i) {
                alloc.destroy(ptr + i);
            }
            if (count == 0) {
                alloc.deallocate_object(ptr, numAlloc);
                ptr = nullptr;
                numAlloc = 0;
            }
        } else if (count > size()) {
            reserve(count);
            for (size_t i = numStored; i < count; ++i) {
                alloc.construct(ptr + i, value);
            }
        }
        numStored = count;
    }

    JTX_HOST
    void swap(vector &other) noexcept {
        ASSERT(alloc != other.alloc);
        std::swap(ptr, other.ptr);
        std::swap(numAlloc, other.numAlloc);
        std::swap(numStored, other.numStored);
    }

#pragma endregion Modifiers

private:
    Allocator alloc;
    Tp *ptr = nullptr;
    size_t numAlloc = 0;
    size_t numStored = 0;
};
#pragma endregion PMR Vector

}// namespace jtx::pmr

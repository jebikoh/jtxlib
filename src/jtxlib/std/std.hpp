#pragma once
#include "jtxlib.hpp"

#include <algorithm>

namespace jtx {
JTX_HOSTDEV
template <typename T>
void swap(T &a, T &b) noexcept {
    T tmp = std::move(a);
    a = std::move(b);
    b = std::move(tmp);
}

template <typename T, int N>
class array {
public:
    using value_type = T;
    using iterator = value_type *;
    using const_iterator = const value_type *;
    using size_t = std::size_t;

    array() = default;

    JTX_HOSTDEV
    array(std::initializer_list<T> v) {
        size_t i = 0;
        for (const T &val: v) _data[i++] = val;
    }

    JTX_HOSTDEV
    void fill (const T &v) {
        for (int i = 0; i < N; ++i) _data[i] = v;
    }

    JTX_HOSTDEV
    bool operator==(const array<T, N> &a) const {
        for (int i = 0; i < N; ++i) {
            if (_data[i] != a[i]) return false;
        }
        return true;
    }

    JTX_HOSTDEV
    bool operator!=(const array<T, N> &a) const { return !(*this == a); }

    JTX_HOSTDEV
    iterator begin() { return _data; }

    JTX_HOSTDEV
    iterator end() { return _data + N; }

    JTX_HOSTDEV
    const_iterator begin() const { return _data; }

    JTX_HOSTDEV
    const_iterator end() const { return _data + N; }

    JTX_HOSTDEV
    size_t size() const { return N; }

    JTX_HOSTDEV
    T &operator[](size_t i) { return _data[i]; }

    JTX_HOSTDEV
    const T &operator[](size_t i) const { return _data[i]; }

    JTX_HOSTDEV
    T *data() { return data; }

    JTX_HOSTDEV
    const T *data() const { return data; }

private:
    T _data[N] = {};
};

// Just like PBRTv4, this is from Google Abseil's span.h
// https://github.com/abseil/abseil-cpp/blob/master/absl/types/span.h
namespace detail {
template <typename C>
constexpr auto GetDataImpl(C& c, char) noexcept  // NOLINT(runtime/references)
    -> decltype(c.data()) {
  return c.data();
}

// Before C++17, std::string::data returns a const char* in all cases.
inline char* GetDataImpl(std::string& s,  // NOLINT(runtime/references)
                         int) noexcept {
  return &s[0];
}

template <typename C>
constexpr auto GetData(C& c) noexcept  // NOLINT(runtime/references)
    -> decltype(GetDataImpl(c, 0)) {
  return GetDataImpl(c, 0);
}

// Detection idioms for size() and data().
template <typename C>
using HasSize =
    std::is_integral<absl::decay_t<decltype(std::declval<C&>().size())>>;

// We want to enable conversion from vector<T*> to Span<const T* const> but
// disable conversion from vector<Derived> to Span<Base>. Here we use
// the fact that U** is convertible to Q* const* if and only if Q is the same
// type or a more cv-qualified version of U.  We also decay the result type of
// data() to avoid problems with classes which have a member function data()
// which returns a reference.
template <typename T, typename C>
using HasData =
    std::is_convertible<absl::decay_t<decltype(GetData(std::declval<C&>()))>*,
                        T* const*>;

// Extracts value type from a Container
template <typename C>
struct ElementType {
  using type = typename absl::remove_reference_t<C>::value_type;
};

template <typename T, size_t N>
struct ElementType<T (&)[N]> {
  using type = T;
};

template <typename C>
using ElementT = typename ElementType<C>::type;

template <typename T>
using EnableIfMutable =
    typename std::enable_if<!std::is_const<T>::value, int>::type;

template <template <typename> class SpanT, typename T>
bool EqualImpl(SpanT<T> a, SpanT<T> b) {
  static_assert(std::is_const<T>::value, "");
  return std::equal(a.begin(), a.end(), b.begin(), b.end());
}

template <template <typename> class SpanT, typename T>
bool LessThanImpl(SpanT<T> a, SpanT<T> b) {
  // We can't use value_type since that is remove_cv_t<T>, so we go the long way
  // around.
  static_assert(std::is_const<T>::value, "");
  return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
}

template <typename From, typename To>
using EnableIfConvertibleTo =
    typename std::enable_if<std::is_convertible<From, To>::value>::type;

// IsView is true for types where the return type of .data() is the same for
// mutable and const instances. This isn't foolproof, but it's only used to
// enable a compiler warning.
template <typename T, typename = void, typename = void>
struct IsView {
  static constexpr bool value = false;
};

template <typename T>
struct IsView<
    T, absl::void_t<decltype(span_internal::GetData(std::declval<const T&>()))>,
    absl::void_t<decltype(span_internal::GetData(std::declval<T&>()))>> {
 private:
  using Container = std::remove_const_t<T>;
  using ConstData =
      decltype(span_internal::GetData(std::declval<const Container&>()));
  using MutData = decltype(span_internal::GetData(std::declval<Container&>()));
 public:
  static constexpr bool value = std::is_same<ConstData, MutData>::value;
};

// These enablers result in 'int' so they can be used as typenames or defaults
// in template parameters lists.
template <typename T>
using EnableIfIsView = std::enable_if_t<IsView<T>::value, int>;

template <typename T>
using EnableIfNotIsView = std::enable_if_t<!IsView<T>::value, int>;
}

template <typename T>
class span {
public:
    using element_type = T;
    using value_type = std::remove_cv_t<T>;
    using pointer = T *;
    using const_pointer = const T *;
    using reference = T &;
    using const_reference = const T &;
    using iterator = T *;
    using const_iterator = const T *;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
private:
    template<typename C>
    using EnableIfConvertibleForm = typename std::enable_if_t<detail::HasData<T, C>::value && detail::HasSize<C>::value>::type;

    template <typename U>
    using EnableIfValueIsConst = typename std::enable_if<std::is_const<T>::value, U>::type;

    template <typename U>
    using EnableIfValueIsMutable = typename std::enable_if<!std::is_const<T>::value, U>::type;

    pointer _ptr;
    size_type len;
public:
#pragma region Constructors
    // Taken from Abseil
    JTX_HOSTDEV
    constexpr span() noexcept : _ptr(nullptr), len(0) {};

    JTX_HOSTDEV
    constexpr span(pointer arr, size_type len) noexcept : _ptr(arr), len(len) {};

    template <size_t N>
    JTX_HOSTDEV
    constexpr span(T (&arr)[N]) noexcept : span(a, N) {} // NOLINT(runtime/explicit)

    JTX_HOSTDEV
    span(std::initializer_list<value_type> v) : span(v.begin(), v.size()) {};

    // Taken from PBRTv4
    template <typename V, typename X = EnableIfConvertibleForm<V>, typename Y = EnableIfValueIsMutable<V>>
    JTX_HOSTDEV
    explicit span(V &v) noexcept : span(v.data(), v.size()) {};

    template <typename V>
    JTX_HOST
    span(std::vector<V> &v) noexcept : span(v.data(), v.size()) {};

    template <typename V>
    JTX_HOST
    span(const std::vector<V> &v) noexcept : span(v.data(), v.size()) {};

    template <typename V, typename X = EnableIfConvertibleFrom<V>,
              typename Y = EnableIfConstView<V>>
    JTX_HOSTDEV
    constexpr span(const V &v) noexcept : span(v.data(), v.size()) {}
#pragma endregion Constructors

    JTX_HOSTDEV
    constexpr pointer data() const noexcept { return _ptr; }

    JTX_HOSTDEV
    constexpr size_type size() const noexcept { return len; }

    JTX_HOSTDEV
    constexpr size_type length() const noexcept { return size(); }

    JTX_HOSTDEV
    constexpr bool empty() const noexcept { return size() == 0; }

    JTX_HOSTDEV
    constexpr reference operator[](size_type i) const {
        ASSERT(i < size());
        return _ptr[i];
    }

    JTX_HOSTDEV
    constexpr const_reference operator[](size_type i) const {
        ASSERT(i < size());
        return _ptr[i];
    }

    JTX_HOSTDEV
    constexpr reference at(size_type i) const {
        ASSERT(i < size());
        *(data() + i);
    }

    JTX_HOSTDEV
    constexpr reference front() const {
        ASSERT(!empty());
        return *data();
    }

    JTX_HOSTDEV
    constexpr reference back() const {
        ASSERT(!empty());
        return *(data() + size() - 1);
    }

    JTX_HOSTDEV
    constexpr iterator begin() const noexcept { return data(); }

    JTX_HOSTDEV
    constexpr const_iterator cbegin() const noexcept { return begin(); }

    JTX_HOSTDEV
    constexpr iterator end() const noexcept { return data() + size(); }

    JTX_HOSTDEV
    constexpr const_iterator cend() const noexcept { return end(); }

    JTX_HOSTDEV
    void remove_prefix(size_type n) noexcept {
        ASSERT(size() >= n);
        _ptr += n;
        len -= n;
    }

    JTX_HOSTDEV
    void remove_suffix(size_type n) noexcept {
        ASSERT(size() >= n);
        len -= n;
    }
};

template<int &...ExplicitArgumentBarrier, typename T>
JTX_HOSTDEV
inline constexpr span<T> makeSpan(T *ptr, size_t len) noexcept {
    return span<T>(ptr, len);
}

template<int &...ExplicitArgumentBarrier, typename T>
JTX_HOSTDEV
inline constexpr span<T> makeSpan(T *begin, T *end) noexcept {
    return span<T>(begin, end - begin);
}

template<int &...ExplicitArgumentBarrier, typename T>
JTX_HOST
inline constexpr span<T> makeSpan(std::vector<T> &v) noexcept {
    return span<T>(v.data(), v.size());
}

template<int &...ExplicitArgumentBarrier, typename C>
JTX_HOSTDEV inline constexpr auto makeSpan(C &c) noexcept -> decltype(makeSpan(detail::GetData(c), c.size())) {
    return makeSpan(detail::GetData(c), c.size());
}

template<int &...ExplicitArgumentBarrier, typename T, size_t N>
JTX_HOSTDEV inline constexpr span<T> makeSpan(T (&arr)[N]) noexcept {
    return span<T>(arr, N);
}

template<int &...ExplicitArgumentBarrier, typename T>
JTX_HOSTDEV
inline constexpr span<const T> makeConstSpan(T *ptr, size_t len) noexcept {
    return span<const T>(ptr, len);
}

template<int &...ExplicitArgumentBarrier, typename T>
JTX_HOSTDEV
inline constexpr span<const T> makeConstSpan(T *begin, T *end) noexcept {
    return span<const T>(begin, end - begin);
}

template<int &...ExplicitArgumentBarrier, typename T>
JTX_HOST
inline constexpr span<const T> makeConstSpan(const std::vector<T> &v) noexcept {
    return span<const T>(v.data(), v.size());
}

template<int &...ExplicitArgumentBarrier, typename C>
JTX_HOSTDEV
inline constexpr auto makeConstSpan(const C &c) noexcept -> decltype(makeSpan(c)) {
    return makeSpan(c);
}

template<int &...ExplicitArgumentBarrier, typename T, size_t N>
JTX_HOSTDEV inline constexpr span<const T> makeConstSpan(const T (&arr)[N]) noexcept {
    return span<const T>(arr, N);
}

}


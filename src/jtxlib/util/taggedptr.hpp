#pragma once

#include <tuple>
#include <jtxlib.hpp>
#include <jtxlib/util/assert.hpp>

/**
 * This the same fat pointer implementation used in PBRTv4 with minor changes
 * https://github.com/mmp/pbrt-v4/blob/39e01e61f8de07b99859df04b271a02a53d9aeb2/src/pbrt/util/taggedptr.h#L736
*/
namespace jtx {
    namespace detail {
        //region const dispatch
        template<typename F, typename R, typename T>
        JTX_HOSTDEV R dispatch(F &&f, const void *ptr, int tag) {
            ASSERT(tag == 0);
            return f((const T *)ptr);
        }

        template<typename F, typename R, typename T0, typename T1>
        JTX_HOSTDEV R dispatch(F &&f, const void *ptr, int tag) {
            ASSERT(tag >= 0 && tag <= 1);
            if (tag == 0) {
                return f((const T0 *)ptr);
            } else {
                return f((const T1 *)ptr);
            }
        }

        template<typename F, typename R, typename T0, typename T1, typename T2>
        JTX_HOSTDEV R dispatch(F &&f, const void *ptr, int tag) {
            ASSERT(tag >= 0 && tag <= 2);
            switch(tag) {
                case 0:
                    return f((const T0 *)ptr);
                case 1:
                    return f((const T1 *)ptr);
                default:
                    return f((const T2 *)ptr);
            }
        }

        template<typename F, typename R, typename T0, typename T1, typename T2, typename T3>
        JTX_HOSTDEV R dispatch(F &&f, const void *ptr, int tag) {
            ASSERT(tag >= 0 && tag <= 3);
            switch(tag) {
                case 0:
                    return f((const T0 *)ptr);
                case 1:
                    return f((const T1 *)ptr);
                case 2:
                    return f((const T2 *)ptr);
                default:
                    return f((const T3 *)ptr);
            }
        }

        template<typename F, typename R, typename T0, typename T1, typename T2, typename T3, typename T4>
        JTX_HOSTDEV R dispatch(F &&f, const void *ptr, int tag) {
            ASSERT(tag >= 0 && tag <= 4);
            switch(tag) {
                case 0:
                    return f((const T0 *)ptr);
                case 1:
                    return f((const T1 *)ptr);
                case 2:
                    return f((const T2 *)ptr);
                case 3:
                    return f((const T3 *)ptr);
                default:
                    return f((const T4 *)ptr);
            }
        }

        template<typename F, typename R, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
        JTX_HOSTDEV R dispatch(F &&f, const void *ptr, int tag) {
            ASSERT(tag >= 0 && tag <= 5);
            switch(tag) {
                case 0:
                    return f((const T0 *)ptr);
                case 1:
                    return f((const T1 *)ptr);
                case 2:
                    return f((const T2 *)ptr);
                case 3:
                    return f((const T3 *)ptr);
                case 4:
                    return f((const T4 *)ptr);
                default:
                    return f((const T5 *)ptr);
            }
        }

        template<typename F, typename R, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
        JTX_HOSTDEV R dispatch(F &&f, const void *ptr, int tag) {
            ASSERT(tag >= 0 && tag <= 6);
            switch(tag) {
                case 0:
                    return f((const T0 *)ptr);
                case 1:
                    return f((const T1 *)ptr);
                case 2:
                    return f((const T2 *)ptr);
                case 3:
                    return f((const T3 *)ptr);
                case 4:
                    return f((const T4 *)ptr);
                case 5:
                    return f((const T5 *)ptr);
                default:
                    return f((const T6 *)ptr);
            }
        }

        template<typename F, typename R, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
        JTX_HOSTDEV R dispatch(F &&f, const void *ptr, int tag) {
            ASSERT(tag >= 0 && tag <= 7);
            switch(tag) {
                case 0:
                    return f((const T0 *)ptr);
                case 1:
                    return f((const T1 *)ptr);
                case 2:
                    return f((const T2 *)ptr);
                case 3:
                    return f((const T3 *)ptr);
                case 4:
                    return f((const T4 *)ptr);
                case 5:
                    return f((const T5 *)ptr);
                case 6:
                    return f((const T6 *)ptr);
                default:
                    return f((const T7 *)ptr);
            }
        }
        //endregion

        //region non-const dispatch
        template<typename F, typename R, typename T>
        JTX_HOSTDEV R dispatch(F &&f, void *ptr, int tag) {
            ASSERT(tag == 0);
            return f((T *)ptr);
        }

        template<typename F, typename R, typename T0, typename T1>
        JTX_HOSTDEV R dispatch(F &&f, void *ptr, int tag) {
            ASSERT(tag >= 0 && tag <= 1);
            if (tag == 0) {
                return f((T0 *)ptr);
            } else {
                return f((T1 *)ptr);
            }
        }

        template<typename F, typename R, typename T0, typename T1, typename T2>
        JTX_HOSTDEV R dispatch(F &&f, void *ptr, int tag) {
            ASSERT(tag >= 0 && tag <= 2);
            switch(tag) {
                case 0:
                    return f((T0 *)ptr);
                case 1:
                    return f((T1 *)ptr);
                default:
                    return f((T2 *)ptr);
            }
        }

        template<typename F, typename R, typename T0, typename T1, typename T2, typename T3>
        JTX_HOSTDEV R dispatch(F &&f, void *ptr, int tag) {
            ASSERT(tag >= 0 && tag <= 3);
            switch(tag) {
                case 0:
                    return f((T0 *)ptr);
                case 1:
                    return f((T1 *)ptr);
                case 2:
                    return f((T2 *)ptr);
                default:
                    return f((T3 *)ptr);
            }
        }

        template<typename F, typename R, typename T0, typename T1, typename T2, typename T3, typename T4>
        JTX_HOSTDEV R dispatch(F &&f, void *ptr, int tag) {
            ASSERT(tag >= 0 && tag <= 4);
            switch(tag) {
                case 0:
                    return f((T0 *)ptr);
                case 1:
                    return f((T1 *)ptr);
                case 2:
                    return f((T2 *)ptr);
                case 3:
                    return f((T3 *)ptr);
                default:
                    return f((T4 *)ptr);
            }
        }

        template<typename F, typename R, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
        JTX_HOSTDEV R dispatch(F &&f, void *ptr, int tag) {
            ASSERT(tag >= 0 && tag <= 5);
            switch(tag) {
                case 0:
                    return f((T0 *)ptr);
                case 1:
                    return f((T1 *)ptr);
                case 2:
                    return f((T2 *)ptr);
                case 3:
                    return f((T3 *)ptr);
                case 4:
                    return f((T4 *)ptr);
                default:
                    return f((T5 *)ptr);
            }
        }

        template<typename F, typename R, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
        JTX_HOSTDEV R dispatch(F &&f, void *ptr, int tag) {
            ASSERT(tag >= 0 && tag <= 6);
            switch(tag) {
                case 0:
                    return f((T0 *)ptr);
                case 1:
                    return f((T1 *)ptr);
                case 2:
                    return f((T2 *)ptr);
                case 3:
                    return f((T3 *)ptr);
                case 4:
                    return f((T4 *)ptr);
                case 5:
                    return f((T5 *)ptr);
                default:
                    return f((T6 *)ptr);
            }
        }

        template<typename F, typename R, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
        JTX_HOSTDEV R dispatch(F &&f, void *ptr, int tag) {
            ASSERT(tag >= 0 && tag <= 7);
            switch(tag) {
                case 0:
                    return f((T0 *)ptr);
                case 1:
                    return f((T1 *)ptr);
                case 2:
                    return f((T2 *)ptr);
                case 3:
                    return f((T3 *)ptr);
                case 4:
                    return f((T4 *)ptr);
                case 5:
                    return f((T5 *)ptr);
                case 6:
                    return f((T6 *)ptr);
                default:
                    return f((T7 *)ptr);
            }
        }
        //endregion

        template<typename... Ts>
        struct IsSameType;

        template<>
        struct IsSameType<> {
            static constexpr bool value = true;
        };

        template<typename T>
        struct IsSameType<T> {
            static constexpr bool value = true;
        };

        template<typename T, typename U, typename...Ts>
        struct IsSameType<T, U, Ts...> {
            static constexpr bool value = (std::is_same_v<T, U> && IsSameType<U, Ts...>::value);
        };

        template<typename... Ts>
        struct SameType;

        template<typename T, typename... Ts>
        struct SameType<T, Ts...> {
            using type = T;
            static_assert(IsSameType<T, Ts...>::value, "Not all types are the same");
        };

        template<typename F, typename... Ts>
        struct ReturnType {
            using type = typename SameType<typename std::invoke_result_t<F, Ts *>...>::type;
        };
    }

    template <typename ...Ts>
    class TaggedPtr {
    public:
      //region Constructors
      JTX_HOSTDEV TaggedPtr() = default;
      JTX_HOSTDEV TaggedPtr(const TaggedPtr &p) : fptr(p.fptr) {}
      JTX_HOSTDEV explicit TaggedPtr(std::nullptr_t ptr) {};

      template <typename T>
      JTX_HOSTDEV explicit TaggedPtr(T* ptr) {
        auto p = reinterpret_cast<uint64_t>(ptr);
        ASSERT((p & PTR_MASK)  == p);
            constexpr unsigned int type = tagIndex<T>();
        fptr = p | ((uint64_t) type << TAG_SHIFT);
      }
      //endregion

      //region Operators
      JTX_HOSTDEV TaggedPtr &operator=(const TaggedPtr &p) {
        fptr = p.fptr;
        return *this;
      }

      JTX_HOSTDEV bool operator==(const TaggedPtr &p) const {
        return fptr == p.fptr;
      }

      JTX_HOSTDEV bool operator!=(const TaggedPtr &p) const {
        return fptr != p.fptr;
      }

        JTX_HOSTDEV
        explicit operator bool() const { return (fptr & PTR_MASK) != 0; }
      //endregion

      //region Getters
      [[nodiscard]] JTX_HOSTDEV unsigned int getTag() const {
        return (fptr & TAG_MASK) >> TAG_SHIFT;
      }

      [[nodiscard]] JTX_HOSTDEV void *getPtr() {
        return reinterpret_cast<void *>(fptr & PTR_MASK);
      }

      [[nodiscard]] JTX_HOSTDEV const void *getPtr() const {
        return reinterpret_cast<const void *>(fptr & PTR_MASK);
      }
      //endregion

      template <typename T>
      [[nodiscard]] JTX_HOSTDEV bool is() const {
        return getTag() == tagIndex<T>();
      }

      //region Casting
      template <typename T>
      JTX_HOSTDEV T *cast() {
        ASSERT(is<T>());
        return reinterpret_cast<T *>(getPtr());
      }

      template <typename T>
      JTX_HOSTDEV const T *cast() const {
        ASSERT(is<T>());
        return reinterpret_cast<const T *>(getPtr());
      }

      template<typename T>
      JTX_HOSTDEV T *castOrNp() {
        if (is<T>()) { return reinterpret_cast<T *>(getPtr()); }
        else { return nullptr; }
      }

      template<typename T>
      JTX_HOSTDEV const T *castOrNp() const {
          if (is<T>()) { return reinterpret_cast<const T *>(getPtr()); }
          else { return nullptr; }
      }
      //endregion

      template <typename T>
      JTX_HOSTDEV static constexpr unsigned int tagIndex() {
        using Tp = typename std::remove_cv_t<T>;
        if constexpr (std::is_same_v<Tp, std::nullptr_t>) return 0;
        else return 1 + getTagIndex<Tp, Ts...>();
      }

      template <typename F>
      JTX_HOSTDEV decltype(auto) dispatch(F &&f) {
        ASSERT(getPtr() != nullptr);
        using R = typename detail::ReturnType<F, Ts...>::type;
        return detail::dispatch<F, R, Ts...>(f, getPtr(), getTag() - 1);
      }

      template <typename F>
      JTX_HOSTDEV decltype(auto) dispatch(F &&f) const {
        ASSERT(getPtr() != nullptr);
        using R = typename detail::ReturnType<F, Ts...>::type;
        return detail::dispatch<F, R, Ts...>(f, getPtr(), getTag() - 1);
      }

    private:
      /*
       * TAG_MASK visualization
       * 1. 0b0000000000000000000000000000000000000000000000000000000000000001
       * 2. 0b0000000000000000000000000000000000000000000000000000000010000000
       * 3. 0b0000000000000000000000000000000000000000000000000000000001111111
       * 4. 0b1111111000000000000000000000000000000000000000000000000000000000
       */
      static constexpr int TAG_SHIFT     = 57;
      static constexpr int TAG_BITS      = 64 - TAG_SHIFT;
      static constexpr uint64_t TAG_MASK = ((1ull << TAG_BITS) - 1) << TAG_SHIFT;
      static constexpr uint64_t PTR_MASK = ~TAG_MASK;

      template <typename T, typename U, typename ...Us>
      JTX_HOSTDEV static constexpr int getTagIndex() {
        if constexpr (std::is_same_v<T, U>) return 0;
        else return 1 + getTagIndex<T, Us...>();
      }

      template <typename T>
      JTX_HOSTDEV static constexpr int getTagIndex() {
        static_assert(!std::is_same_v<T, T>, "Type not found in list");
        return 0;
      }

      uint64_t fptr = 0;
    };
}
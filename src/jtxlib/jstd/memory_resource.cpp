#include "memory_resource.hpp"

#include <new>

#if defined(JTXLIB__ALIGNED_MALLOC)
#include <malloc.h>// NOLINT(*-deprecated-headers)
#elif defined(JTXLIB_POSIX_MEMALIGN)
#include <stdlib.h>// NOLINT(*-deprecated-headers)
#endif

namespace jtx::pmr {

class newdel_res_t final : public memory_resource {
    void *do_allocate(size_t bytes, size_t alignment) override {
        if (bytes == 0) return nullptr;
#if defined(JTXLIB__ALIGNED_MALLOC)
        return _aligned_malloc(bytes, alignment);
#elif defined(JTXLIB_POSIX_MEMALIGN)
        void *ptr;
        if (alignment < sizeof(void *)) return malloc(bytes);
        if (posix_memalign(&ptr, alignment, bytes) != 0) ptr = nullptr;
        return ptr;
#else
        return memalign(alignment, size);
#endif
    }

    void do_deallocate(void *p, size_t bytes, size_t alignment) override {
        if (!p) return;
#if defined(JTXLIB__ALIGNED_MALLOC)
        _aligned_free(p);
#else
        free(p);
#endif
    }

    [[nodiscard]]
    bool do_is_equal(const memory_resource &other) const noexcept override {
        return this == &other;
    }
};

class null_res_t final : public memory_resource {
    void *do_allocate(size_t bytes, size_t alignment) override { throw std::bad_alloc(); }

    void do_deallocate(void *p, size_t bytes, size_t alignment) noexcept override {}

    [[nodiscard]]
    bool do_is_equal(const memory_resource &other) const noexcept override { return this == &other; }
};

static newdel_res_t *newdel_res;
static null_res_t *null_res;
static memory_resource *def = new_delete_resource();

memory_resource *new_delete_resource() noexcept {
    if (!newdel_res) newdel_res = new newdel_res_t;
    return newdel_res;
}

memory_resource *null_memory_resource() noexcept {
    if (!null_res) null_res = new null_res_t;
    return null_res;
}

memory_resource *set_default_resource(memory_resource *r) noexcept {
    memory_resource *old = def;
    def = r;
    return old;
}

memory_resource *get_default_resource() noexcept { return def; }

}// namespace jtx::pmr
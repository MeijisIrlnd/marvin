// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include <marvin/memory/marvin_Pool.h>
namespace marvin::memory {
    template <typename T, bool AllowGrowth = true>
    class PoolAllocator {
    public:
        typedef T value_type;

        PoolAllocator(size_t size) noexcept : m_memoryPool(size) {
        }

        T* allocate(size_t n) {
            if constexpr (AllowGrowth) {
                auto* allocation = m_memoryPool.alloc();
                return allocation;
            } else {
                auto allocOpt = m_memoryPool.tryAlloc();
                if (!allocOpt) return nullptr;
                return *allocOpt;
            }
        }


        void deallocate(T* toDealloc, size_t n) noexcept {
        }


        template <class U, class... Args>
        void construct(U* p, Args&&... args) {
            [[maybe_unused]] auto* res = std::construct_at(p, std::forward<Args...>(args...));
        }

    private:
        MemoryPool<T> m_memoryPool;
    };
} // namespace marvin::memory
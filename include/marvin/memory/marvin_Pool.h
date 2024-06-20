// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include <marvin/library/marvin_Literals.h>
#include <vector>
#include <optional>
#include <memory>
#include <cassert>

namespace marvin::memory {

    /**
        \brief An arena-allocator-esque memory pool.

        Allow for pseudo-dynamic object creation / destruction in a realtime safe context.\n
        The pool is specialised for a single type T, as all returned objects must be the same size.
        Follows RAII - on destruction, each "allocated" object in the pool's destructor will be called before the arena is freed.
        Heavily inspired by [this implementation](https://thinkingeek.com/2017/11/19/simple-memory-pool/) and [this article](https://www.rfleury.com/p/untangling-lifetimes-the-arena-allocator)
        (thanks Rachel!!)
     */
    template <typename T>
    class MemoryPool final {
    public:
        /**
            Constructs a memory pool, with sizeof(T) * arenaSize bytes preallocated.
            \param arenaSize The number of objects to preallocate storage for.
         */
        explicit MemoryPool(size_t arenaSize) : m_arenaSize(arenaSize) {
            assert(arenaSize != 0);
            m_arenas.emplace_back(new Arena(arenaSize));
            m_freeList = m_arenas.front()->data();
        }

        /**
            Mass "delete"s all elements in the memory pool, freeing up the memory for future use.
         */
        void clear() {
            for (auto& arena : m_arenas) {
                arena->clear();
            }
        }

        /**
            Checks if there's enough memory remaining to allocate another T, without allocating a new arena.
            \return true if enough space is remaining, false otherwise
         */
        [[nodiscard]] bool hasFreeSpace() const noexcept {
            return m_freeList;
        }

        T* alloc() {
            if (!m_freeList) {
            }
        }
        /**
            Allocates (in inverted commas) a new object of type T. If the current arena is out of memory, a new one will be created,
            so if using in a realtime context, maybe actually use `tryAlloc` instead.
            \param args The arguments to be forwarded to T's constructor.
            \return A pointer to an object of type T, created using preallocated memory
         */
        template <typename... Args>
        T* alloc(Args&&... args) {
            if (!m_freeList) {
                allocateArena();
            }
            return allocInternal(std::forward<Args>(args)...);
        }

        /**
            Allocates (in inverted commas) a new object of type T, if the arena has enough memory allocated to do so. As it will never allocate,
            suitable for use in a realtime context.
            \param args The arguments to be forwarded to T's constructor.
            \return An optional containing a pointer to our new T if the pool had enough memory, nullopt otherwise.
         */
        template <typename... Args>
        std::optional<T*> tryAlloc(Args&&... args) {
            if (!m_freeList) return {};
            auto* res = allocInternal(std::forward<Args>(args)...);
            return res;
        }

        /**
            Calls toFree's destructor, and marks the memory used by `toFree` as free, allowing reuse in future calls to alloc.
            \param toFree A pointer to the variable to free. Obviously *must* have been created by this MemoryPool.
         */
        void free(T* toFree) {
            toFree->T::~T();
            auto* asBlock = Block::fromItem(toFree);
            asBlock->setNext(m_freeList);
            asBlock->setFree(true);
            m_freeList = asBlock;
        }

    private:
        void allocateArena() {
            m_arenas.emplace_back(std::make_unique<Arena>(m_arenaSize));
            m_freeList = m_arenas[m_arenas.size() - 1]->data();
        }

        template <typename... Args>
        T* allocInternal(Args&&... args) {
            auto* current = m_freeList;
            m_freeList = current->next();
            T* contents = current->get();
            current->setFree(false);
            new (contents) T(std::forward<Args>(args)...);
            return contents;
        }

        union Block;
        struct BlockInfo {
            Block* next{ nullptr };
            bool inUse{ false };
        };
        union Block {
            Block() = default;
            [[nodiscard]] Block* next() const noexcept {
                return m_info.next;
            }

            void setNext(Block* next) noexcept {
                m_info.next = next;
            }

            [[nodiscard]] T* get() {
                return reinterpret_cast<T*>(m_data);
            }

            [[nodiscard]] bool isFree() const noexcept {
                return m_info.inUse;
            }

            void setFree(bool isFree_) noexcept {
                m_info.inUse = isFree_;
            }

            static Block* fromItem(T* item) {
                auto* asBlock = reinterpret_cast<Block*>(item);
                return asBlock;
            }

        private:
            BlockInfo m_info{};
            char m_data[sizeof(T)];
        };


        struct Arena {
            explicit Arena(size_t n) : m_nBlocks(n), m_blocks(new Block[n]) {
                for (auto i = 1_sz; i < n; ++i) {
                    m_blocks[i - 1].setNext(&m_blocks[i]);
                }
            }
            // When an arena gets destroyed, none of the in-use destructors get called.
            ~Arena() noexcept {
                clear();
            }

            void clear() {
                for (auto i = 0_sz; i < m_nBlocks; ++i) {
                    if (!m_blocks[i].isFree()) {
                        m_blocks[i].setFree(true);
                        auto* asT = m_blocks[i].get();
                        asT->T::~T();
                    }
                }
            }

            Block* data() const {
                return m_blocks.get();
            }

        private:
            const size_t m_nBlocks;
            std::unique_ptr<Block[]> m_blocks;
        };
        const size_t m_arenaSize;
        std::vector<std::unique_ptr<Arena>> m_arenas;
        Block* m_freeList{ nullptr };
    };
} // namespace marvin::memory

#pragma once

#include <memory>
#include <cassert>
#include <list>
#include <cstring>
#include <random>
#include <algorithm>

template <typename T, unsigned max_level>
struct skiplist_node {

    T _t;
    skiplist_node *_fp[max_level] = {nullptr};
    skiplist_node *_bp = nullptr;

    skiplist_node() = default;

    skiplist_node(const skiplist_node &other)
            : _t(other._t)
            , _bp(other._bp) {
        memcpy(_fp, other._fp, max_level);
    }

};

template <
        typename T,
        unsigned max_level,
        template <typename, unsigned> class Node,
        typename Alloc = std::allocator<Node<T, max_level>>
>
class base_skiplist : private Alloc {
public:
    using node_type = Node<T, max_level>;

    template <typename NodeType, typename U>
    struct base_iterator: public std::iterator<std::input_iterator_tag, U>  {
        using node_type = NodeType;
        node_type *_iter = nullptr;
    public:
        base_iterator() = default;
        explicit base_iterator(node_type *iter) : _iter(iter) {}
        base_iterator(const base_iterator &) = default;

        U &operator*() {
            return _iter->_t;
        }

        inline base_iterator operator++() {
            if (_iter){
                _iter = _iter->_fp[0];
            }
            return *this;
        }

        inline base_iterator operator++(int) {
            base_iterator cur(*this);
            if (_iter) {
                _iter = _iter->_fp[0];
            }
            return cur;
        }

        inline bool operator==(const base_iterator &rhs) {
            return _iter == rhs._iter;
        }

        inline bool operator!=(const base_iterator &rhs) {
            return !((*this) == rhs);
        }

        inline U &operator->() {
            return _iter->_t;
        }

    };

    using iterator = base_iterator<Node<T, max_level>, T>;
    using const_iterator = base_iterator<const Node<T, max_level>, const T>;

private:
    node_type *_roots[max_level] = { nullptr };
    size_t _size = 0;

    node_type *predecessors(const T &target, node_type **predecessors[]) {
        unsigned level = max_level - 1;
        node_type *ret = nullptr;
        for (int i = 0; i < max_level; i++) {
            predecessors[i] = &_roots[i];
        }
        for (;;) {
            if (!level && *predecessors[0] && target == (*predecessors[0])->_t) {
                if (!ret) ret = *predecessors[0];
            }

            if (!*predecessors[level] || target <= (*predecessors[level])->_t) {
                if (level) {
                    --level;
                } else {
                    break;
                }
            } else {
                predecessors[level] = &((*predecessors[level])->_fp[level]);
            }
        }
        return ret;
    }

    void insert_node(node_type *node) {
        ++_size;
        node_type **precs[max_level];
        predecessors(node->_t, precs);
        unsigned level = random_level();
        for (auto l = 0; l <= level; l++) {
            node_type *next = *precs[l];
            node->_fp[l] = next;
            *precs[l] = node;
        }
    }

    unsigned random_level() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::geometric_distribution<> d;
        do {
            auto ret = d(gen);
            if (ret < max_level) {
                return ret;
            }
        } while (true);
    }

public:
    base_skiplist() = default;

    base_skiplist(const base_skiplist &other) {
        for (auto  &&e : other) {
            insert(e);
        }
    }

    base_skiplist(base_skiplist &&other) noexcept : _size(other._size) {
        memcpy(_roots, other._roots, sizeof(_roots));
        other._size = 0;
        std::fill(other._roots, other._roots + max_level, nullptr);
    }

    base_skiplist (std::initializer_list<T> ilist) {
        for (auto const &elem : ilist) {
            insert(elem);
        }
    }

    base_skiplist &operator=(const base_skiplist &rhs) {
        base_skiplist clone(rhs);
        this->~base_skiplist();
        new(this) base_skiplist(std::move(clone));
        return *this;
    }

    base_skiplist &operator=(base_skiplist &&rhs) {
        this->~base_skiplist();
        new(this) base_skiplist(std::move(clone));
        return *this;
    }


    iterator find(const T &elem) {
        node_type **ignore[max_level];
        return iterator{ predecessors(elem, ignore) };
    }

    iterator insert(const T &elem) {
        node_type *curr = this->allocate(1);
        this->construct(curr);
        new(&curr->_t) T(elem);
        insert_node(curr);
        return iterator(curr);
    }

    template <typename ...Args>
    iterator emplace(Args &&...args) {
        node_type *curr = this->allocate(1);
        this->construct(curr);
        new(&curr->_t) T(std::forward<Args>(args)...);
        insert_node(curr);
        return iterator(curr);
    }

    size_t erase(const T &elem) {
        node_type **precs[max_level];
        size_t removed = 0;
        for (;;) {
            node_type *self = predecessors(elem, precs);
            if (!self) return removed;
            for (int l = 0; l < max_level; l++) {
                if (*precs[l] == self) {
                    *precs[l] = self->_fp[l];
                }
            }
            this->destroy(self);
            this->deallocate(self, 1);
            ++removed;
        }
    }

    iterator begin() {
        return iterator{ _roots[0] };
    }

    iterator end() {
        return {};
    }

    const_iterator begin() const {
        return const_iterator{ _roots[0] };
    }

    const_iterator end() const {
        return {};
    }

    size_t size() {
        return _size;
    }


    ~base_skiplist() {
        if (_roots[0]) {
            node_type *node = _roots[0];
            while (node) {
                node_type *next = node->_fp[0];
                this->destroy(node);
                this->deallocate(node, 1);
                node = next;
            }
        }
    }

};

template <typename T, unsigned max_level>
using skiplist = base_skiplist<T, max_level, skiplist_node>;


template <typename T, unsigned max_level>
struct skipset_node;

template <typename T, unsigned max_level>
class skipset : public base_skiplist<T, max_level, skipset_node> {};
#pragma once

#include <memory>
#include <assert.h>
#include <list>

template <typename T, unsigned max_level>
struct skiplist_node {
    unsigned _level = 0;
    T _t;
    skiplist_node *_fp[max_level] = {nullptr};
    skiplist_node *_bp = nullptr;

    inline skiplist_node *next(unsigned level) {
        assert(level < max_level);
        return _fp[level];
    }
};

template <
        typename T,
        unsigned max_level,
        template <typename, unsigned> typename Node,
        typename Alloc = std::allocator<Node<T, max_level>>,
        typename Comp = std::less<T>,
        Comp comp = declval<Comp>()
>
class base_skiplist : private Alloc {
public:
    using node_type = Node<T, max_level>;

    template <typename T>
    struct base_iterator: public std::iterator<std::input_iterator_tag, T>  {
        using node_type = Node<T, max_level>;
        node_type *_iter = nullptr;
    public:
        base_iterator() = default;
        base_iterator(node_type *iter) : _iter(iter) {}
        base_iterator(const base_iterator &) = default;

        T &operator*() {
            return _iter->_t;
        }

        inline base_iterator operator++() {
            _iter = _iter->next(0);
            return *this;
        }

        inline base_iterator operator++(int) {
            base_iterator cur(*this);
            _iter = _iter->next(0);
            return cur;
        }

        inline bool operator==(const base_iterator &rhs) {
            return _iter == rhs._iter;
        }

        inline bool operator!=(const base_iterator &rhs) {
            return !lhs == rhs;
        }

        inline T &operator->() {
            return _iter->_t;
        }

    };

    using iterator = base_iterator<T>;
    using const_iterator = base_iterator<const T>;

private:
    node_type *_root;
    unsigned _level = 0;

    node_type *lower_bound(T &elem) {
        node_type *curr = _root;
        unsinged level = _level;
        while (curr) {
            node_type *next = curr->next(level);
            while (!next && level) {
                next = curr->next(--level);
            }

            return
        }
        while (comp(curr->_t, elem)) {
            if (level)
        }
    }

public:

    base_skiplist() = default;

    base_skiplist(const base_skiplist &) {

    }

    base_skiplist(base_skiplist &&other): _root(other._root), _level(other._level) {
        other.root = nullptr;
    }

    iterator find(const T &elem) {

    }

    iterator insert(const T &elem) {
        node_type
    }


    ~skiplist() {
        if (_root) {
            node_type *node = _root;
            while (node) {
                node_type *next = node->_fp[0];
                this->destroy(node);
                this->deallocate(node);
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
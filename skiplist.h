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

    skiplist_node() = default;

    skiplist_node(const skiplist_node &other)
            : _level(other._level)
            , _t(other._t)
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

    template <typename U>
    struct base_iterator: public std::iterator<std::input_iterator_tag, U>  {
        using node_type = Node<U, max_level>;
        node_type *_iter = nullptr;
    public:
        base_iterator() = default;
        base_iterator(node_type *iter) : _iter(iter) {}
        base_iterator(const base_iterator &) = default;

        U &operator*() {
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
            return !(*this) == rhs;
        }

        inline U &operator->() {
            return _iter->_t;
        }

    };

    using iterator = base_iterator<T>;
    using const_iterator = base_iterator<const T>;

private:
    node_type *_root;
    unsigned _level = 0;

    bool precedents(const T &target, node_type **precedents) {
        if (!_root) return false;
        node_type *curr = _root;
        unsigned level = _level;
        for (;;) {
            node_type *next = curr->next(level);
            if (!next || target < next->_t) {
                /* we can start with the lower level */
                precedents[level] = curr;
                if (level) {
                    /* if we have another level to go */
                    --level;
                } else {
                    /* we have the bound */
                    break;
                }
            } else {
                curr = next;
            }
        }
        return true;
    }

public:

    base_skiplist() = default;

    base_skiplist(const base_skiplist &) {

    }

    base_skiplist(base_skiplist &&other): _root(other._root), _level(other._level) {
        other.root = nullptr;
    }

    iterator find(const T &elem) {
        node_type *precs[max_level];
        precedents(elem, precs);
        if (precs[0]->_t == elem) return iterator(precs[0]);
        return {};
    }

    iterator insert(const T &elem) {
        node_type *curr = this->allocate(1);
        this->construct(curr);
        new(&curr->_t) T(elem);
        precedents(elem, curr->_fp);
        return iterator(curr);
    }

    template <typename ...Args>
    iterator emplace(Args ...args) {
        node_type *curr = this->allocate(1);
        this->construct(curr);
        new(&curr->_t) T(std::forward<Args...>(std::move(args)...));
        precedents(curr->_t, curr->_fp);
        return iterator(curr);
    }


    ~base_skiplist() {
        if (_root) {
            node_type *node = _root;
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
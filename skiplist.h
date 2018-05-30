#pragma once

#include <memory>
#include <cassert>
#include <list>
#include <cstring>
#include <random>

static std::random_device rd;
static std::mt19937 gen(rd());

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
            _iter = _iter->_fp[0];
            return *this;
        }

        inline base_iterator operator++(int) {
            base_iterator cur(*this);
            _iter = _iter->_fp[0];
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

    using iterator = base_iterator<T>;
    using const_iterator = base_iterator<const T>;

private:
    node_type *_roots[max_level] = { nullptr };

    node_type *precedents(const T &target, node_type **precedents[]) {
        unsigned level = max_level - 1;
        node_type *ret = nullptr;
        for (int i = 0; i < max_level; i++) {
            precedents[i] = &_roots[i];
        }
        for (;;) {
            if (!level && *precedents[0] && target == (*precedents[0])->_t) {
                if (!ret) ret = *precedents[0];
            }

            if (!*precedents[level] || target < (*precedents[level])->_t) {
                if (level) {
                    --level;
                } else {
                    break;
                }
            } else {
                precedents[level] = &((*precedents[level])->_fp[level]);
            }
        }
        return ret;
    }

    unsigned random_level() {
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

    base_skiplist(const base_skiplist &) {

    }

    base_skiplist(base_skiplist &&other) {
        memcpy(_roots, other._roots, sizeof(_roots));
    }

    iterator find(const T &elem) {
        node_type **ignore[max_level];
        return {precedents(elem, ignore)};
    }

    iterator insert(const T &elem) {
        node_type **precs[max_level];
        precedents(elem, precs);
        node_type *curr = this->allocate(1);
        this->construct(curr);
        new(&curr->_t) T(elem);
        unsigned level = random_level();
        for (auto l = 0; l <= level; l++) {
            node_type *next = *precs[l];
            curr->_fp[l] = next;
            *precs[l] = curr;
        }
        return iterator(curr);
    }

    template <typename ...Args>
    iterator emplace(Args ...args) {
        node_type *curr = this->allocate(1);
        this->construct(curr);
        new(&curr->_t) T(std::forward<Args...>(std::move(args)...));
        node_type **precs[max_level];
        precedents(curr->_t, precs);
        unsigned level = random_level();
        for (auto l = 0; l <= level; l++) {
            node_type *next = *precs[l];
            curr->next[l] = next;
            *precs[l] = curr;
        }
        return iterator(curr);
    }

    iterator begin() {
        return { _roots[0] };
    }

    iterator end() {
        return {};
    }

    const_iterator cbegin() {
        return { _roots[0] };
    }

    const_iterator cend() {
        return {};
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
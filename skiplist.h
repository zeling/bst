#pragma once

#include <memory>

template <typename T, unsigned max_level>
struct skiplist_node {
    unsigned _level = 0;
    T _t;
    skiplist_node *_fp[max_level];

    inline skiplist_node *next(unsigned level) {
        return _fp[level];
    }
};

template <
        typename T,
        unsigned max_level,
        template <typename, unsigned> typename Node,
        typename Alloc = std::allocator<Node<T, max_level>>,
        typename Comp = std::less<T> >
class base_skiplist : private Alloc {
public:
    using node_type = Node<T, max_level>;

private:
    node_type *_root;
    unsigned _level = 0;



    node_type *lower_bound(T &elem) {
        node_type *curr = _root;
        for (unsigned level = _level; level >= 0; --level) {
            node_type *next = curr->next(level);

        }
    }

public:
    base_skiplist() = default;

    base_skiplist(const base_skiplist &) {

    }

    base_skiplist(base_skiplist &&other): _root(other._root), _level(other._level) {
        other.root = nullptr;
    }

    ~skiplist() {
        if (_root) {
            node_type *node = _root;
            while (node) {
                node_type *next = node->_fp[0];
                this->destroy(node);
                node = next;
            }
        }
    }

};

template <typename T, unsigned max_level>
using skiplist = base_skiplist<T, max_level, skiplist_node>;
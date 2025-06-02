#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <memory>
#include <random>
#include <limits>
#include <stdexcept>
#include <iterator>
#include <cassert>

template<typename T>
class SkipList {
    static_assert(std::is_copy_constructible_v<T>, "T must be copy constructible");

private:
    struct Node {
        T value;
        std::vector<Node*> forward;

        Node(int level, const T& val) : value(val), forward(level, nullptr) {}
    };

    static constexpr int MAX_LEVEL = 16;
    float probability_ = 0.5f;

    Node* head_;
    int level_;
    size_t size_;
    std::default_random_engine gen_;
    std::uniform_real_distribution<float> dist_;

    int randomLevel() {
        int lvl = 1;
        while (dist_(gen_) < probability_ && lvl < MAX_LEVEL) ++lvl;
        return lvl;
    }

public:
    SkipList()
        : head_(new Node(MAX_LEVEL, T{})), level_(0), size_(0), dist_(0.0f, 1.0f) {}

    ~SkipList() {
        clear();
        delete head_;
    }

    void clear() {
	    Node* current = head_->forward[0];
	    while (current) {
	        Node* next = current->forward[0];
	        delete current;
	        current = next;
	    }

	    std::fill(head_->forward.begin(), head_->forward.end(), nullptr);
	    level_ = 0;
	    size_ = 0;
	}

    bool insert(const T& value) {
	    if constexpr (std::is_pointer_v<T>) {
	        if (value == nullptr) {
	            throw std::invalid_argument("Cannot insert null pointer into SkipList");
	        }
	    }

	    std::vector<Node*> update(MAX_LEVEL + 1, nullptr);
	    Node* current = head_;

	    for (int i = level_; i >= 0; --i) {
	        while (current->forward[i] && current->forward[i]->value < value) {
	            current = current->forward[i];
	        }
	        update[i] = current;
	    }

	    current = current->forward[0];

	    if (current && current->value == value) {
	        return false; 
	    }

	    int newLevel = randomLevel();
	    if (newLevel > level_) {
	        for (int i = level_ + 1; i <= newLevel; ++i) {
	            update[i] = head_;
	        }
	        level_ = newLevel;
	    }

	    Node* newNode = new Node(newLevel + 1, value);
	    for (int i = 0; i <= newLevel; ++i) {
	        newNode->forward[i] = update[i]->forward[i];
	        update[i]->forward[i] = newNode;
	    }

	    ++size_;
	    return true;
	}


    bool erase(const T& value) {
	    if (empty()) {
	        throw std::logic_error("Cannot erase from an empty SkipList");
	    }

	    std::vector<Node*> update(MAX_LEVEL + 1, nullptr);
	    Node* current = head_;

	    for (int i = level_; i >= 0; --i) {
	        while (current->forward[i] && current->forward[i]->value < value) {
	            current = current->forward[i];
	        }
	        update[i] = current;
	    }

	    current = current->forward[0];

	    if (!current || current->value != value) {
	        return false; 
	    }

	    for (int i = 0; i <= level_; ++i) {
	        if (update[i]->forward[i] != current) break;
	        update[i]->forward[i] = current->forward[i];
	    }

	    delete current;

	    while (level_ > 0 && head_->forward[level_] == nullptr) {
	        --level_;
	    }

	    --size_;
	    return true;
	}


    bool contains(const T& value) const {
	    Node* current = head_;

	    for (int i = level_; i >= 0; --i) {
	        while (current->forward[i] && current->forward[i]->value < value) {
	            current = current->forward[i];
	        }
	    }

	    current = current->forward[0];
	    return current && current->value == value;
	}

    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    class iterator {
        Node* node_;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = T;
        using reference         = T&;
        using pointer           = T*;
        using difference_type   = std::ptrdiff_t;

        iterator(Node* node) : node_(node) {}

        T& operator*() const {
		    if (!node_) {
		        throw std::out_of_range("Dereferencing end() iterator");
		    }
		    return node_->value;
		}

        T* operator->() const { return &node_->value; }

        iterator& operator++() {
            if (node_) node_ = node_->forward[0];
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const iterator& a, const iterator& b) { return a.node_ == b.node_; }
        friend bool operator!=(const iterator& a, const iterator& b) { return !(a == b); }
    };

    iterator begin() const { return iterator(head_->forward[0]); }
    iterator end() const { return iterator(nullptr); }
};

#endif
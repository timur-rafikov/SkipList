/**
 * @file skip_list.hpp
 * @brief Template implementation of the Skip List data structure
 * 
 * 
 * @author Тимур Рафиков 24.Б81
 * @date 02.06.2025
 */

#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <memory>
#include <random>
#include <limits>
#include <stdexcept>
#include <iterator>
#include <cassert>

/**
 * @class SkipList
 * @brief A template class for a list with omissions.
 * 
 * @tparam T The type of items stored in the list. Must be comparable to the one being copied.
 */
template<typename T>
class SkipList {
    static_assert(std::is_copy_constructible_v<T>, "T must be copy constructible");

private:
	/**
     * @struct Node
     * @brief A node of the SkipList structure containing a value and a list of pointers to other levels.
     */
    struct Node {
        T value; ///< The value stored in the node
        std::vector<Node*> forward; ///< Pointers to the next nodes on each level

        /**
         * @brief Node constructor.
         * @param level Number of levels.
         * @param val Node value.
         */
        Node(int level, const T& val) : value(val), forward(level, nullptr) {}
    };

    static constexpr int MAX_LEVEL = 16; ///< Maximum number of levels in the list
    float probability_ = 0.5f;			 ///< Probability of increasing the level

    Node* head_;						 ///< Head of the list (initial dummy node)
    int level_;							 ///< Current maximum level
    size_t size_;						 ///< Number of elements
    std::default_random_engine gen_;			///< Random number generator
    std::uniform_real_distribution<float> dist_;///< Distribution for level generation

    /**
     * @brief Generates a random level based on the specified probability.
     * @return A level from 0 to MAX_LEVEL - 1.
     */
    int randomLevel() {
        int lvl = 1;
        while (dist_(gen_) < probability_ && lvl < MAX_LEVEL) ++lvl;
        return lvl;
    }

public:
	/// Default constructor
    SkipList()
        : head_(new Node(MAX_LEVEL, T{})), level_(0), size_(0), dist_(0.0f, 1.0f) {}

    /// Destructor
    ~SkipList() {
        clear();
        delete head_;
    }

    /**
     * @brief Clears the list.
     */
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

	/**
     * @brief Inserts a value into the list.
     * @param value The value to insert.
     * @return true if the value was inserted; false if it already exists.
     * @throw std::invalid_argument if trying to insert a nullptr (for pointer types).
     */
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


	/**
     * @brief Deletes a value from the list.
     * @param value The value to delete.
     * @return true if the value was deleted; false if not found.
     * @throw std::logic_error if trying to delete from an empty list.
     */
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


	/**
     * @brief Checks if a value exists in the list.
     * @param value The value to search for.
     * @return true if the value is found; otherwise false.
     */
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

	/**
     * @brief Returns the number of elements in the list.
     * @return The size of the list.
     */
    size_t size() const { return size_; }

    /**
     * @brief Checks if the list is empty.
     * @return true if the list is empty.
     */
    bool empty() const { return size_ == 0; }

    /**
     * @class iterator
     * @brief Iterator for the SkipList.
     */
    class iterator {
        Node* node_;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = T;
        using reference         = T&;
        using pointer           = T*;
        using difference_type   = std::ptrdiff_t;

        /**
         * @brief Iterator constructor.
         * @param node Pointer to the current node.
         */
        iterator(Node* node) : node_(node) {}

        /// Dereference operator
        T& operator*() const {
		    if (!node_) {
		        throw std::out_of_range("Dereferencing end() iterator");
		    }
		    return node_->value;
		}

		/// Member access operator
        T* operator->() const { return &node_->value; }

        /// Prefix increment
        iterator& operator++() {
            if (node_) node_ = node_->forward[0];
            return *this;
        }

        /// Postfix increment
        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        /// Equality comparison
        friend bool operator==(const iterator& a, const iterator& b) { return a.node_ == b.node_; }
        /// Inequality comparison
        friend bool operator!=(const iterator& a, const iterator& b) { return !(a == b); }
    };

    /**
     * @brief Returns an iterator to the beginning of the list.
     */
    iterator begin() const { return iterator(head_->forward[0]); }
    
    /**
     * @brief Returns an iterator to the end of the list.
     */
    iterator end() const { return iterator(nullptr); }
};

#endif

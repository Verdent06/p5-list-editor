#ifndef LIST_HPP
#define LIST_HPP
/* List.hpp
 *
 * Doubly-linked, double-ended list with Iterator interface.
 * EECS 280 List/Editor Project
 */

#include <cassert>  // assert
#include <iterator>  // std::bidirectional_iterator_tag


template <typename T>
class List {
  // OVERVIEW: a doubly-linked, double-ended list with Iterator interface
public:

  // EFFECTS: returns true if the list is empty
  bool empty() const { return list_size == 0; }

  // EFFECTS: returns the number of elements in this List
  // HINT: traversing a list is slow; keep the size in a member variable
  //       (same idea as std::list).
  int size() const { return list_size; }

  // REQUIRES: list is not empty
  // EFFECTS: returns the first element in the list by reference
  T &front() { return first->datum; }

  // REQUIRES: list is not empty
  // EFFECTS: returns the last element in the list by reference
  T &back() { return last->datum; }

  // EFFECTS: inserts datum at the front of the list
  void push_front(const T &datum);

  // EFFECTS: inserts datum at the back of the list
  void push_back(const T &datum);

  // REQUIRES: list is not empty
  // MODIFIES: invalidates all iterators to the removed element
  // EFFECTS: removes the item at the front of the list
  void pop_front();

  // REQUIRES: list is not empty
  // MODIFIES: invalidates all iterators to the removed element
  // EFFECTS: removes the item at the back of the list
  void pop_back();

  // MODIFIES: invalidates all iterators to the removed elements
  // EFFECTS: removes all items from the list
  void clear();

  List();
  ~List();
  List(const List &other);
  List &operator=(const List &rhs);

private:
  // Private node type for the intrusive list.
  struct Node {
    Node *next;
    Node *prev;
    T datum;
  };

  // REQUIRES: list is empty
  // EFFECTS: copies all nodes from other into this list
  void copy_all(const List<T> &other);

  // first points to the first node, or nullptr if empty
  Node *first;
  // last points to the last node, or nullptr if empty
  Node *last;
  int list_size;

public:
  class Iterator {
    friend class List;

  public:
    // OVERVIEW: Iterator interface to List

    Iterator() : list_ptr(nullptr), node_ptr(nullptr) {}

    // Type aliases required for STL algorithms. Do not modify these.
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T *;
    using reference = T &;

    T &operator*() const {
      assert(node_ptr);
      return node_ptr->datum;
    }

    Iterator &operator++() {
      assert(list_ptr);
      assert(node_ptr);
      node_ptr = node_ptr->next;
      return *this;
    }

    Iterator operator++(int /*dummy*/) {
      Iterator copy = *this;
      ++(*this);
      return copy;
    }

    bool operator==(const Iterator &rhs) const {
      return list_ptr == rhs.list_ptr and node_ptr == rhs.node_ptr;
    }

    bool operator!=(const Iterator &rhs) const {
      return not (*this == rhs);
    }

    // This operator will be used to test your code. Do not modify it.
    // REQUIRES: Iterator is decrementable. All iterators for a list are
    //           decrementable, including end iterators, except begin().
    // EFFECTS: moves this iterator to the previous element; returns *this
    Iterator &operator--() {
      assert(list_ptr);
      assert(*this != list_ptr->begin());
      if (node_ptr) {
        node_ptr = node_ptr->prev;
        return *this;
      }
      // Decrementing end moves to the last element.
      node_ptr = list_ptr->last;
      return *this;
    }

    // This operator will be used to test your code. Do not modify it.
    // REQUIRES: Iterator is decrementable (same as prefix --).
    // EFFECTS: moves this iterator to the previous element; returns the
    //          original iterator value
    Iterator operator--(int /*dummy*/) {
      Iterator copy = *this;
      operator--();
      return copy;
    }

    // REQUIRES: Iterator is dereferenceable
    // EFFECTS: returns a pointer to the underlying element
    // NOTE: T must be a class type to use it->member. If T has a field f,
    //       it->f accesses f on the underlying element.
    T *operator->() const {
      return &operator*();
    }

  private:
    // List this iterator is associated with
    const List *list_ptr;
    // Current position in the list (nullptr means end())
    Node *node_ptr;

    // Iterator at a specific position in the given list
    Iterator(const List *list_ptr_in, Node *node_ptr_in);

  };  // Iterator

  // EFFECTS: returns an iterator to the first element
  Iterator begin() const { return Iterator(this, first); }

  // EFFECTS: returns an iterator past the last element
  Iterator end() const { return Iterator(this, nullptr); }

  // REQUIRES: i is valid and dereferenceable for this list
  // MODIFIES: invalidates iterators to the removed element
  // EFFECTS: removes the element at i; returns an iterator to the element
  //          that followed the erased one
  Iterator erase(Iterator i);

  // REQUIRES: i is a valid iterator for this list
  // EFFECTS: inserts datum before the element at i; returns an iterator to
  //          the new element
  Iterator insert(Iterator i, const T &datum);

};  // List


// =============================================================================
// Member function implementations
// =============================================================================

template <typename T>
List<T>::List() : first(nullptr), last(nullptr), list_size(0) {}

template <typename T>
List<T>::~List() {
  clear();
}

template <typename T>
List<T>::List(const List &other)
    : first(nullptr), last(nullptr), list_size(0) {
  copy_all(other);
}

template <typename T>
List<T> &List<T>::operator=(const List &rhs) {
  if (this == &rhs) {
    return *this;
  }
  clear();
  copy_all(rhs);
  return *this;
}

template <typename T>
void List<T>::copy_all(const List<T> &other) {
  assert(empty());
  for (Node *node = other.first; node; node = node->next) {
    push_back(node->datum);
  }
}

template <typename T>
void List<T>::push_front(const T &datum) {
  Node *new_node = new Node;
  new_node->datum = datum;
  new_node->prev = nullptr;
  if (empty()) {
    new_node->next = nullptr;
    first = last = new_node;
    ++list_size;
    return;
  }
  new_node->next = first;
  first->prev = new_node;
  first = new_node;
  ++list_size;
}

template <typename T>
void List<T>::push_back(const T &datum) {
  Node *new_node = new Node;
  new_node->datum = datum;
  new_node->next = nullptr;
  if (empty()) {
    new_node->prev = nullptr;
    first = last = new_node;
    ++list_size;
    return;
  }
  new_node->prev = last;
  last->next = new_node;
  last = new_node;
  ++list_size;
}

template <typename T>
void List<T>::pop_front() {
  assert(not empty());
  Node *removed = first;
  if (first == last) {
    first = nullptr;
    last = nullptr;
  } else {
    first = first->next;
    first->prev = nullptr;
  }
  delete removed;
  --list_size;
}

template <typename T>
void List<T>::pop_back() {
  assert(not empty());
  Node *removed = last;
  if (first == last) {
    first = nullptr;
    last = nullptr;
  } else {
    last = last->prev;
    last->next = nullptr;
  }
  delete removed;
  --list_size;
}

template <typename T>
void List<T>::clear() {
  while (not empty()) {
    pop_front();
  }
}

template <typename T>
List<T>::Iterator::Iterator(const List *list_ptr_in, Node *node_ptr_in)
    : list_ptr(list_ptr_in), node_ptr(node_ptr_in) {}

template <typename T>
typename List<T>::Iterator List<T>::erase(Iterator i) {
  assert(i.list_ptr == this);
  assert(i.node_ptr);

  if (i.node_ptr == first) {
    pop_front();
    return begin();
  }
  if (i.node_ptr == last) {
    pop_back();
    return end();
  }

  Node *current = i.node_ptr;
  Node *next_node = current->next;
  current->prev->next = next_node;
  next_node->prev = current->prev;
  delete current;
  --list_size;
  return Iterator(this, next_node);
}

template <typename T>
typename List<T>::Iterator List<T>::insert(Iterator i, const T &datum) {
  assert(i.list_ptr == this);

  if (i.node_ptr == first) {
    push_front(datum);
    return begin();
  }
  if (not i.node_ptr) {
    push_back(datum);
    return Iterator(this, last);
  }

  Node *current = i.node_ptr;
  Node *prev_node = current->prev;
  Node *new_node = new Node;
  new_node->datum = datum;
  new_node->next = current;
  new_node->prev = prev_node;
  prev_node->next = new_node;
  current->prev = new_node;
  ++list_size;
  return Iterator(this, new_node);
}


#endif  // Do not remove this. Write all your code above this line.

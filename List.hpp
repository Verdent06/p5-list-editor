#ifndef LIST_HPP
#define LIST_HPP
/* List.hpp
 *
 * doubly-linked, double-ended list with Iterator interface
 * EECS 280 List/Editor Project
 */

#include <iterator> //std::bidirectional_iterator_tag
#include <cassert>  //assert


template <typename T>
class List {
  //OVERVIEW: a doubly-linked, double-ended list with Iterator interface
public:

  //EFFECTS:  returns true if the list is empty
  bool empty() const { return list_size == 0; }

  //EFFECTS: returns the number of elements in this List
  //HINT:    Traversing a list is really slow. Instead, keep track of the size
  //         with a private member variable. That's how std::list does it.
  int size() const { return list_size; }

  //REQUIRES: list is not empty
  //EFFECTS: Returns the first element in the list by reference
  T & front() { return first->datum; }

  //REQUIRES: list is not empty
  //EFFECTS: Returns the last element in the list by reference
  T & back() { return last->datum; }

  //EFFECTS:  inserts datum into the front of the list
  void push_front(const T &datum);

  //EFFECTS:  inserts datum into the back of the list
  void push_back(const T &datum);

  //REQUIRES: list is not empty
  //MODIFIES: invalidates all iterators to the removed element
  //EFFECTS:  removes the item at the front of the list
  void pop_front();

  //REQUIRES: list is not empty
  //MODIFIES: invalidates all iterators to the removed element
  //EFFECTS:  removes the item at the back of the list
  void pop_back();

  //MODIFIES: invalidates all iterators to the removed elements
  //EFFECTS:  removes all items from the list
  void clear();

  List();
  ~List();
  List(const List &other);
  List &operator=(const List &rhs);

private:
  //a private type
  struct Node {
    Node *next;
    Node *prev;
    T datum;
  };

  //REQUIRES: list is empty
  //EFFECTS:  copies all nodes from other to this
  void copy_all(const List<T> &other);

  Node *first;   // points to first Node in list, or nullptr if list is empty
  Node *last;    // points to last Node in list, or nullptr if list is empty
  int list_size;

public:
  ////////////////////////////////////////
  class Iterator {
    friend class List;

  public:
    //OVERVIEW: Iterator interface to List

    Iterator() : list_ptr(nullptr), node_ptr(nullptr) {}

    // Type aliases required to work with STL algorithms. Do not modify these.
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

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
      return list_ptr == rhs.list_ptr && node_ptr == rhs.node_ptr;
    }

    bool operator!=(const Iterator &rhs) const {
      return !(*this == rhs);
    }

    // This operator will be used to test your code. Do not modify it.
    // REQUIRES: Iterator is decrementable. All iterators associated with a
    //           list are decrementable, including end iterators, except for
    //           begin iterators (those equal to begin() on the list)
    // EFFECTS:  moves this Iterator to point to the previous element
    //           and returns a reference to this Iterator
    Iterator& operator--() { // prefix -- (e.g. --it)
      assert(list_ptr);
      assert(*this != list_ptr->begin());
      if (node_ptr) {
        node_ptr = node_ptr->prev;
      } else { // decrementing an end Iterator moves it to the last element
        node_ptr = list_ptr->last;
      }
      return *this;
    }

    // This operator will be used to test your code. Do not modify it.
    // REQUIRES: Iterator is decrementable. All iterators associated with a
    //           list are decrementable, including end iterators, except for
    //           begin iterators (those equal to begin() on the list)
    // EFFECTS:  moves this Iterator to point to the previous element
    //           and returns a copy of the original Iterator
    Iterator operator--(int /*dummy*/) { // postfix -- (e.g. it--)
      Iterator copy = *this;
      operator--();
      return copy;
    }

    // REQUIRES: Iterator is dereferenceable
    // EFFECTS: returns the underlying element by pointer
    // NOTE: T must be a class type to use the syntax it->. If T has a
    //       member variable f, then it->f accesses f on the
    //       underlying T element.
    T* operator->() const {
      return &operator*();
    }

  private:
    const List *list_ptr; //pointer to the List associated with this Iterator
    Node *node_ptr; //current Iterator position is a List node

    // construct an Iterator at a specific position in the given List
    Iterator(const List *lp, Node *np);

  };//List::Iterator
  ////////////////////////////////////////

  // return an Iterator pointing to the first element
  Iterator begin() const { return Iterator(this, first); }

  // return an Iterator pointing to "past the end"
  Iterator end() const { return Iterator(this, nullptr); }

  //REQUIRES: i is a valid, dereferenceable iterator associated with this list
  //MODIFIES: invalidates all iterators to the removed element
  //EFFECTS: Removes a single element from the list container.
  //         Returns An iterator pointing to the element that followed the
  //         element erased by the function call
  Iterator erase(Iterator i);

  //REQUIRES: i is a valid iterator associated with this list
  //EFFECTS: Inserts datum before the element at the specified position.
  //         Returns an iterator to the the newly inserted element.
  Iterator insert(Iterator i, const T &datum);

};//List


////////////////////////////////////////////////////////////////////////////////
// Member function implementations

template <typename T>
List<T>::List() : first(nullptr), last(nullptr), list_size(0) {}

template <typename T>
List<T>::~List() {
  clear();
}

template <typename T>
List<T>::List(const List &other) : first(nullptr), last(nullptr), list_size(0) {
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
  for (Node *n = other.first; n != nullptr; n = n->next) {
    push_back(n->datum);
  }
}

template <typename T>
void List<T>::push_front(const T &datum) {
  Node *n = new Node;
  n->datum = datum;
  n->prev = nullptr;
  if (empty()) {
    n->next = nullptr;
    first = last = n;
  } else {
    n->next = first;
    first->prev = n;
    first = n;
  }
  ++list_size;
}

template <typename T>
void List<T>::push_back(const T &datum) {
  Node *n = new Node;
  n->datum = datum;
  n->next = nullptr;
  if (empty()) {
    n->prev = nullptr;
    first = last = n;
  } else {
    n->prev = last;
    last->next = n;
    last = n;
  }
  ++list_size;
}

template <typename T>
void List<T>::pop_front() {
  assert(!empty());
  Node *old = first;
  if (first == last) {
    first = nullptr;
    last = nullptr;
  } else {
    first = first->next;
    first->prev = nullptr;
  }
  delete old;
  --list_size;
}

template <typename T>
void List<T>::pop_back() {
  assert(!empty());
  Node *old = last;
  if (first == last) {
    first = nullptr;
    last = nullptr;
  } else {
    last = last->prev;
    last->next = nullptr;
  }
  delete old;
  --list_size;
}

template <typename T>
void List<T>::clear() {
  while (!empty()) {
    pop_front();
  }
}

template <typename T>
List<T>::Iterator::Iterator(const List *lp, Node *np)
    : list_ptr(lp), node_ptr(np) {}

template <typename T>
typename List<T>::Iterator List<T>::erase(Iterator i) {
  assert(i.list_ptr == this);
  assert(i.node_ptr != nullptr);

  if (i.node_ptr == first) {
    pop_front();
    return begin();
  }
  if (i.node_ptr == last) {
    pop_back();
    return end();
  }

  Node *cur = i.node_ptr;
  Node *nxt = cur->next;
  cur->prev->next = nxt;
  nxt->prev = cur->prev;
  delete cur;
  --list_size;
  return Iterator(this, nxt);
}

template <typename T>
typename List<T>::Iterator List<T>::insert(Iterator i, const T &datum) {
  assert(i.list_ptr == this);

  if (i.node_ptr == first) {
    push_front(datum);
    return begin();
  }
  if (i.node_ptr == nullptr) {
    push_back(datum);
    return Iterator(this, last);
  }

  Node *cur = i.node_ptr;
  Node *prv = cur->prev;
  Node *n = new Node;
  n->datum = datum;
  n->next = cur;
  n->prev = prv;
  prv->next = n;
  cur->prev = n;
  ++list_size;
  return Iterator(this, n);
}


#endif // Do not remove this. Write all your code above this line.

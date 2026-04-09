#include "List.hpp"
#include "unit_test_framework.hpp"

#include <vector>

// Collect list elements in forward order (helps ASSERT_SEQUENCE_EQUAL and prints well).
template <typename T>
static std::vector<T> list_to_vector(const List<T> &lst) {
  std::vector<T> out;
  for (auto it = lst.begin(); it != lst.end(); ++it) {
    out.push_back(*it);
  }
  return out;
}

// ---------------------------------------------------------------------------
// 1. Core functionality & size
// ---------------------------------------------------------------------------

TEST(default_constructor_empty) {
  List<int> lst;
  ASSERT_EQUAL(lst.size(), 0);
  ASSERT_TRUE(lst.empty());
}

TEST(push_front_one_element) {
  List<int> lst;
  lst.push_front(7);
  ASSERT_EQUAL(lst.size(), 1);
  ASSERT_FALSE(lst.empty());
  ASSERT_EQUAL(lst.front(), 7);
  ASSERT_EQUAL(lst.back(), 7);
}

TEST(push_front_multiple) {
  List<int> lst;
  lst.push_front(1);
  lst.push_front(2);
  lst.push_front(3);
  ASSERT_EQUAL(lst.size(), 3);
  ASSERT_EQUAL(lst.front(), 3);
  ASSERT_EQUAL(lst.back(), 1);
  ASSERT_SEQUENCE_EQUAL(list_to_vector(lst), std::vector<int>({3, 2, 1}));
}

TEST(push_back_one_element) {
  List<int> lst;
  lst.push_back(4);
  ASSERT_EQUAL(lst.size(), 1);
  ASSERT_EQUAL(lst.front(), 4);
  ASSERT_EQUAL(lst.back(), 4);
}

TEST(push_back_multiple) {
  List<int> lst;
  lst.push_back(10);
  lst.push_back(20);
  lst.push_back(30);
  ASSERT_EQUAL(lst.size(), 3);
  ASSERT_EQUAL(lst.front(), 10);
  ASSERT_EQUAL(lst.back(), 30);
  ASSERT_SEQUENCE_EQUAL(list_to_vector(lst), std::vector<int>({10, 20, 30}));
}

TEST(pop_front_sequence) {
  List<int> lst;
  lst.push_back(1);
  lst.push_back(2);
  lst.push_back(3);

  lst.pop_front();
  ASSERT_EQUAL(lst.size(), 2);
  ASSERT_EQUAL(lst.front(), 2);
  ASSERT_EQUAL(lst.back(), 3);

  lst.pop_front();
  ASSERT_EQUAL(lst.size(), 1);
  ASSERT_EQUAL(lst.front(), 3);
  ASSERT_EQUAL(lst.back(), 3);

  lst.pop_front();
  ASSERT_EQUAL(lst.size(), 0);
  ASSERT_TRUE(lst.empty());
}

TEST(pop_back_sequence) {
  List<int> lst;
  lst.push_back(1);
  lst.push_back(2);
  lst.push_back(3);

  lst.pop_back();
  ASSERT_EQUAL(lst.size(), 2);
  ASSERT_EQUAL(lst.front(), 1);
  ASSERT_EQUAL(lst.back(), 2);

  lst.pop_back();
  ASSERT_EQUAL(lst.size(), 1);
  ASSERT_EQUAL(lst.front(), 1);
  ASSERT_EQUAL(lst.back(), 1);

  lst.pop_back();
  ASSERT_EQUAL(lst.size(), 0);
  ASSERT_TRUE(lst.empty());
}

TEST(clear_then_push) {
  List<int> lst;
  lst.push_back(1);
  lst.push_front(0);
  ASSERT_FALSE(lst.empty());

  lst.clear();
  ASSERT_EQUAL(lst.size(), 0);
  ASSERT_TRUE(lst.empty());

  lst.push_back(9);
  ASSERT_EQUAL(lst.size(), 1);
  ASSERT_EQUAL(lst.front(), 9);
  ASSERT_EQUAL(lst.back(), 9);
}

// ---------------------------------------------------------------------------
// 2. Big three (memory & deep copying)
// ---------------------------------------------------------------------------

TEST(copy_constructor_deep_copy) {
  List<int> orig;
  orig.push_back(1);
  orig.push_back(2);
  orig.push_back(3);

  List<int> cpy(orig);

  cpy.pop_front();
  ASSERT_SEQUENCE_EQUAL(list_to_vector(orig), std::vector<int>({1, 2, 3}));
  ASSERT_SEQUENCE_EQUAL(list_to_vector(cpy), std::vector<int>({2, 3}));

  auto it = cpy.begin();
  ++it;
  *it = 99;
  ASSERT_SEQUENCE_EQUAL(list_to_vector(orig), std::vector<int>({1, 2, 3}));
  ASSERT_SEQUENCE_EQUAL(list_to_vector(cpy), std::vector<int>({2, 99}));
}

TEST(assignment_deep_copy_independent) {
  List<int> a;
  a.push_back(5);
  a.push_back(6);

  List<int> b;
  b.push_back(100);
  b.push_back(200);
  b.push_back(300);

  a = b;

  ASSERT_SEQUENCE_EQUAL(list_to_vector(b), std::vector<int>({100, 200, 300}));

  b.pop_back();
  auto it = a.begin();
  ++it;
  *it = 999;

  ASSERT_SEQUENCE_EQUAL(list_to_vector(b), std::vector<int>({100, 200}));
  ASSERT_SEQUENCE_EQUAL(list_to_vector(a), std::vector<int>({100, 999, 300}));
}

TEST(self_assignment_preserves_contents) {
  List<int> lst;
  lst.push_back(1);
  lst.push_back(2);
  lst.push_back(3);

  List<int> *p = &lst;
  lst = *p;

  ASSERT_EQUAL(lst.size(), 3);
  ASSERT_SEQUENCE_EQUAL(list_to_vector(lst), std::vector<int>({1, 2, 3}));
}

// ---------------------------------------------------------------------------
// 3. Iterators (traversal & operators)
// ---------------------------------------------------------------------------

TEST(iterator_forward_traversal) {
  List<int> lst;
  lst.push_back(2);
  lst.push_back(4);
  lst.push_back(6);

  std::vector<int> seen;
  for (auto it = lst.begin(); it != lst.end(); ++it) {
    seen.push_back(*it);
  }
  ASSERT_SEQUENCE_EQUAL(seen, std::vector<int>({2, 4, 6}));
}

TEST(iterator_backward_from_end) {
  List<int> lst;
  lst.push_back(1);
  lst.push_back(2);
  lst.push_back(3);

  std::vector<int> rev;
  for (auto it = lst.end(); it != lst.begin();) {
    --it;
    rev.push_back(*it);
  }
  ASSERT_SEQUENCE_EQUAL(rev, std::vector<int>({3, 2, 1}));
}

TEST(iterator_prefix_postfix_increment) {
  List<int> lst;
  lst.push_back(10);
  lst.push_back(20);
  lst.push_back(30);

  auto it = lst.begin();

  auto post = it++;
  ASSERT_EQUAL(*post, 10);
  ASSERT_EQUAL(*it, 20);

  auto &pref = ++it;
  ASSERT_EQUAL(*pref, 30);
  ASSERT_EQUAL(*it, 30);
  ++it;
  ASSERT_TRUE(it == lst.end());
}

TEST(iterator_prefix_postfix_decrement) {
  List<int> lst;
  lst.push_back(7);
  lst.push_back(8);

  auto it = lst.end();
  auto post = it--;
  ASSERT_TRUE(post == lst.end());
  ASSERT_EQUAL(*it, 8);

  auto &pref = --it;
  ASSERT_EQUAL(*pref, 7);
  ASSERT_TRUE(it == lst.begin());
}

TEST(iterator_equality_inequality) {
  List<int> lst;
  lst.push_back(1);
  lst.push_back(2);

  auto a = lst.begin();
  auto b = lst.begin();
  ASSERT_TRUE(a == b);
  ASSERT_FALSE(a != b);

  ++b;
  ASSERT_FALSE(a == b);
  ASSERT_TRUE(a != b);

  auto e1 = lst.end();
  auto e2 = lst.end();
  ASSERT_TRUE(e1 == e2);
}

// ---------------------------------------------------------------------------
// 4. Insert & erase (edge cases)
// ---------------------------------------------------------------------------

TEST(insert_empty_list_at_begin) {
  List<int> lst;
  auto ret = lst.insert(lst.begin(), 42);
  ASSERT_EQUAL(lst.size(), 1);
  ASSERT_EQUAL(lst.front(), 42);
  ASSERT_EQUAL(lst.back(), 42);
  ASSERT_TRUE(ret == lst.begin());
  ASSERT_EQUAL(*ret, 42);
}

TEST(insert_front_middle_back_return_values) {
  List<int> lst;
  lst.push_back(10);
  lst.push_back(20);
  lst.push_back(30);

  auto r_front = lst.insert(lst.begin(), 5);
  ASSERT_TRUE(r_front == lst.begin());
  ASSERT_EQUAL(*r_front, 5);
  ASSERT_SEQUENCE_EQUAL(list_to_vector(lst), std::vector<int>({5, 10, 20, 30}));

  auto mid = lst.begin();
  ++mid;
  ++mid;
  auto r_mid = lst.insert(mid, 15);
  ASSERT_EQUAL(*r_mid, 15);
  ASSERT_SEQUENCE_EQUAL(list_to_vector(lst),
                        std::vector<int>({5, 10, 15, 20, 30}));

  auto r_back = lst.insert(lst.end(), 40);
  ASSERT_EQUAL(*r_back, 40);
  ASSERT_EQUAL(lst.back(), 40);
  ASSERT_SEQUENCE_EQUAL(list_to_vector(lst),
                        std::vector<int>({5, 10, 15, 20, 30, 40}));
}

TEST(erase_front_middle_back_returns) {
  List<int> lst;
  lst.push_back(1);
  lst.push_back(2);
  lst.push_back(3);
  lst.push_back(4);

  auto after_front = lst.erase(lst.begin());
  ASSERT_TRUE(after_front == lst.begin());
  ASSERT_EQUAL(*after_front, 2);
  ASSERT_SEQUENCE_EQUAL(list_to_vector(lst), std::vector<int>({2, 3, 4}));

  auto mid = lst.begin();
  ++mid;
  auto after_mid = lst.erase(mid);
  ASSERT_EQUAL(*after_mid, 4);
  ASSERT_SEQUENCE_EQUAL(list_to_vector(lst), std::vector<int>({2, 4}));

  auto last = lst.begin();
  ++last;
  auto after_last = lst.erase(last);
  ASSERT_TRUE(after_last == lst.end());
  ASSERT_SEQUENCE_EQUAL(list_to_vector(lst), std::vector<int>({2}));
}

TEST(erase_only_element_empties_list) {
  List<int> lst;
  lst.push_back(100);
  ASSERT_EQUAL(lst.size(), 1);

  auto after = lst.erase(lst.begin());
  ASSERT_TRUE(after == lst.end());
  ASSERT_EQUAL(lst.size(), 0);
  ASSERT_TRUE(lst.empty());
}

TEST_MAIN()

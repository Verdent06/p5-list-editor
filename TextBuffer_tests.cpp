#include "TextBuffer.hpp"
#include "unit_test_framework.hpp"

using namespace std;

TEST(test_default_constructor) {
  TextBuffer buffer;

  ASSERT_TRUE(buffer.is_at_end());
  ASSERT_EQUAL(buffer.size(), 0);
  ASSERT_EQUAL(buffer.get_row(), 1);
  ASSERT_EQUAL(buffer.get_column(), 0);
  ASSERT_EQUAL(buffer.get_index(), 0);
  ASSERT_EQUAL(buffer.stringify(), "");
}

TEST(test_insert_one_character) {
  TextBuffer buffer;
  buffer.insert('a');

  ASSERT_TRUE(buffer.is_at_end());
  ASSERT_EQUAL(buffer.size(), 1);
  ASSERT_EQUAL(buffer.stringify(), "a");
  ASSERT_EQUAL(buffer.get_row(), 1);
  ASSERT_EQUAL(buffer.get_column(), 1);
  ASSERT_EQUAL(buffer.get_index(), 1);
}

TEST(test_backward_moves_to_last_character) {
  TextBuffer buffer;
  buffer.insert('a');
  buffer.insert('b');
  buffer.insert('c');

  bool moved = buffer.backward();

  ASSERT_TRUE(moved);
  ASSERT_FALSE(buffer.is_at_end());
  ASSERT_EQUAL(buffer.data_at_cursor(), 'c');
  ASSERT_EQUAL(buffer.get_row(), 1);
  ASSERT_EQUAL(buffer.get_column(), 2);
  ASSERT_EQUAL(buffer.get_index(), 2);
}

TEST(test_forward_moves_to_end) {
  TextBuffer buffer;
  buffer.insert('a');
  buffer.insert('b');

  buffer.backward();
  bool moved = buffer.forward();

  ASSERT_TRUE(moved);
  ASSERT_TRUE(buffer.is_at_end());
  ASSERT_EQUAL(buffer.get_row(), 1);
  ASSERT_EQUAL(buffer.get_column(), 2);
  ASSERT_EQUAL(buffer.get_index(), 2);
}

TEST(test_remove_character_at_cursor) {
  TextBuffer buffer;
  buffer.insert('a');
  buffer.insert('b');
  buffer.insert('c');

  buffer.backward();
  buffer.backward();
  bool removed = buffer.remove();

  ASSERT_TRUE(removed);
  ASSERT_EQUAL(buffer.stringify(), "ac");
  ASSERT_EQUAL(buffer.size(), 2);
  ASSERT_EQUAL(buffer.data_at_cursor(), 'c');
  ASSERT_EQUAL(buffer.get_row(), 1);
  ASSERT_EQUAL(buffer.get_column(), 1);
  ASSERT_EQUAL(buffer.get_index(), 1);
}

TEST(test_remove_at_end_returns_false) {
  TextBuffer buffer;
  buffer.insert('a');

  bool removed = buffer.remove();

  ASSERT_FALSE(removed);
  ASSERT_TRUE(buffer.is_at_end());
  ASSERT_EQUAL(buffer.stringify(), "a");
}

TEST(test_insert_newline_updates_position) {
  TextBuffer buffer;
  buffer.insert('a');
  buffer.insert('\n');

  ASSERT_TRUE(buffer.is_at_end());
  ASSERT_EQUAL(buffer.stringify(), "a\n");
  ASSERT_EQUAL(buffer.get_row(), 2);
  ASSERT_EQUAL(buffer.get_column(), 0);
  ASSERT_EQUAL(buffer.get_index(), 2);
}

TEST(test_backward_over_newline_updates_row_and_column) {
  TextBuffer buffer;
  buffer.insert('a');
  buffer.insert('\n');
  buffer.insert('b');

  buffer.backward();
  ASSERT_EQUAL(buffer.data_at_cursor(), 'b');
  ASSERT_EQUAL(buffer.get_row(), 2);
  ASSERT_EQUAL(buffer.get_column(), 0);

  buffer.backward();
  ASSERT_EQUAL(buffer.data_at_cursor(), '\n');
  ASSERT_EQUAL(buffer.get_row(), 1);
  ASSERT_EQUAL(buffer.get_column(), 1);
  ASSERT_EQUAL(buffer.get_index(), 1);
}

TEST(test_move_to_row_start_moves_to_first_character_in_row) {
  TextBuffer buffer;
  buffer.insert('a');
  buffer.insert('b');
  buffer.insert('\n');
  buffer.insert('c');
  buffer.insert('d');

  buffer.move_to_row_start();

  ASSERT_FALSE(buffer.is_at_end());
  ASSERT_EQUAL(buffer.data_at_cursor(), 'c');
  ASSERT_EQUAL(buffer.get_row(), 2);
  ASSERT_EQUAL(buffer.get_column(), 0);
  ASSERT_EQUAL(buffer.get_index(), 3);
}

TEST(test_move_to_row_end_moves_to_end_of_row) {
  TextBuffer buffer;
  buffer.insert('a');
  buffer.insert('b');
  buffer.insert('\n');
  buffer.insert('c');
  buffer.insert('d');

  buffer.backward();
  buffer.backward();
  buffer.move_to_row_end();

  ASSERT_TRUE(buffer.is_at_end());
  ASSERT_EQUAL(buffer.get_row(), 2);
  ASSERT_EQUAL(buffer.get_column(), 2);
  ASSERT_EQUAL(buffer.get_index(), 5);
}

TEST(test_move_to_column_moves_within_row) {
  TextBuffer buffer;
  buffer.insert('a');
  buffer.insert('b');
  buffer.insert('c');
  buffer.insert('d');

  buffer.backward();
  buffer.backward();
  buffer.backward();
  buffer.backward();

  buffer.move_to_column(2);

  ASSERT_FALSE(buffer.is_at_end());
  ASSERT_EQUAL(buffer.data_at_cursor(), 'c');
  ASSERT_EQUAL(buffer.get_row(), 1);
  ASSERT_EQUAL(buffer.get_column(), 2);
  ASSERT_EQUAL(buffer.get_index(), 2);
}

TEST(test_up_moves_to_previous_row_same_column) {
  TextBuffer buffer;
  buffer.insert('a');
  buffer.insert('b');
  buffer.insert('c');
  buffer.insert('\n');
  buffer.insert('d');
  buffer.insert('e');
  buffer.insert('f');

  buffer.backward();
  buffer.backward();

  bool moved = buffer.up();

  ASSERT_TRUE(moved);
  ASSERT_EQUAL(buffer.data_at_cursor(), 'b');
  ASSERT_EQUAL(buffer.get_row(), 1);
  ASSERT_EQUAL(buffer.get_column(), 1);
}

TEST(test_down_moves_to_next_row_same_column) {
  TextBuffer buffer;
  buffer.insert('a');
  buffer.insert('b');
  buffer.insert('c');
  buffer.insert('\n');
  buffer.insert('d');
  buffer.insert('e');
  buffer.insert('f');

  buffer.backward();
  buffer.backward();
  buffer.up();

  bool moved = buffer.down();

  ASSERT_TRUE(moved);
  ASSERT_EQUAL(buffer.data_at_cursor(), 'e');
  ASSERT_EQUAL(buffer.get_row(), 2);
  ASSERT_EQUAL(buffer.get_column(), 1);
}

TEST(test_up_on_first_row_returns_false) {
  TextBuffer buffer;
  buffer.insert('a');
  buffer.insert('b');
  buffer.backward();

  bool moved = buffer.up();

  ASSERT_FALSE(moved);
  ASSERT_EQUAL(buffer.data_at_cursor(), 'b');
  ASSERT_EQUAL(buffer.get_row(), 1);
  ASSERT_EQUAL(buffer.get_column(), 1);
}

TEST(test_down_to_shorter_row_moves_to_row_end) {
  TextBuffer buffer;
  buffer.insert('a');
  buffer.insert('b');
  buffer.insert('c');
  buffer.insert('d');
  buffer.insert('\n');
  buffer.insert('x');
  buffer.insert('y');

  buffer.backward();
  buffer.backward();
  buffer.backward();
  buffer.backward();
  buffer.backward();

  bool moved = buffer.down();

  ASSERT_TRUE(moved);
  ASSERT_TRUE(buffer.is_at_end());
  ASSERT_EQUAL(buffer.get_row(), 2);
  ASSERT_EQUAL(buffer.get_column(), 2);
  ASSERT_EQUAL(buffer.get_index(), 7);
}

TEST_MAIN()
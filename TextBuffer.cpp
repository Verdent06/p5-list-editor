/* TextBuffer.cpp
 *
 * Member definitions for TextBuffer (EECS 280 List/Editor Project).
 */

#include "TextBuffer.hpp"

#include <string>

namespace {

// Newline ends a logical row in the buffer (matches TextBuffer.hpp invariants).
constexpr char kNewline = '\n';

}  // namespace

TextBuffer::TextBuffer()
    : cursor(data.end()), row(1), column(0), index(0) {}

int TextBuffer::compute_column() const {
  // Count characters back to the previous newline or buffer start without
  // stepping onto the line-ending '\n', so past-the-end positions stay correct.
  int column_count = 0;
  Iterator position = cursor;
  while (position != data.begin()) {
    Iterator before = position;
    --before;
    if (*before == kNewline) {
      break;
    }
    position = before;
    ++column_count;
  }
  return column_count;
}

bool TextBuffer::forward() {
  if (cursor == data.end()) {
    return false;
  }
  if (*cursor == kNewline) {
    ++row;
    column = 0;
  } else {
    ++column;
  }
  ++index;
  ++cursor;
  return true;
}

bool TextBuffer::backward() {
  if (cursor == data.begin()) {
    return false;
  }
  --cursor;
  --index;
  if (*cursor == kNewline) {
    --row;
    column = compute_column();
    return true;
  }
  --column;
  return true;
}

void TextBuffer::insert(char character) {
  data.insert(cursor, character);
  if (character == kNewline) {
    ++row;
    column = 0;
  } else {
    ++column;
  }
  ++index;
}

bool TextBuffer::remove() {
  if (cursor == data.end()) {
    return false;
  }
  cursor = data.erase(cursor);
  return true;
}

void TextBuffer::move_to_row_start() {
  while (column > 0) {
    backward();
  }
}

void TextBuffer::move_to_row_end() {
  while (cursor != data.end() and *cursor != kNewline) {
    forward();
  }
}

void TextBuffer::move_to_column(int new_column) {
  while (new_column < column) {
    backward();
  }
  while (column < new_column) {
    if (cursor == data.end()) {
      break;
    }
    if (*cursor == kNewline) {
      break;
    }
    if (not forward()) {
      break;
    }
  }
}

bool TextBuffer::up() {
  if (row == 1) {
    return false;
  }
  int target_column = column;
  move_to_row_start();
  backward();
  move_to_row_start();
  move_to_column(target_column);
  return true;
}

bool TextBuffer::down() {
  int target_column = column;
  int row_before = row;
  move_to_row_end();
  if (not forward()) {
    return false;
  }
  if (row == row_before) {
    return false;
  }
  move_to_column(target_column);
  return true;
}

bool TextBuffer::is_at_end() const {
  return cursor == data.end();
}

char TextBuffer::data_at_cursor() const {
  return *cursor;
}

int TextBuffer::get_row() const {
  return row;
}

int TextBuffer::get_column() const {
  return column;
}

int TextBuffer::get_index() const {
  return index;
}

int TextBuffer::size() const {
  return static_cast<int>(data.size());
}

std::string TextBuffer::stringify() const {
  return std::string(data.begin(), data.end());
}

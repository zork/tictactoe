////
// key_event.h
////

#pragma once

#include <string>

class KeyEvent {
 public:
  enum Type {
    kKeyDown,
    kKeyUp,
    kKeyChar,
  };

  KeyEvent(Type type, const std::string& value, int key_code);
  ~KeyEvent();

  Type type() const { return type_; }
  const std::string& value() const { return value_; }
  int key_code() const { return key_code_; }

 private:
  Type type_;
  std::string value_;
  int key_code_;
};

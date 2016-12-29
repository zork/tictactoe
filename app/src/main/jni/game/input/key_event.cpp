////
// key_event.h
////

#include "game/input/key_event.h"

KeyEvent::KeyEvent(Type type, const std::string& value, int key_code)
    : type_(type), value_(value), key_code_(key_code) {}

KeyEvent::~KeyEvent() {}

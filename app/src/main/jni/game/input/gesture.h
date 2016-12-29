////
// gesture.h
////

#pragma once

#include "game/input/input_listener.h"

class Gesture : public InputListener {
 public:
  Gesture();
  virtual ~Gesture();

  void SetId(long id) { id_ = id; }
  long id() { return id_; }

 private:
  long id_;
};

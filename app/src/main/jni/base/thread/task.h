////
// task.h
////

#pragma once

#include "base/macros.h"

#include <memory>

// A task to be run by the message queue.
class Task {
 public:
  Task();
  virtual ~Task();

  virtual void Execute() = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(Task);
};

// A task for deleting an object.
template <typename T>
class DeleteTask : public Task {
 public:
  DeleteTask(T* ptr) : ptr_(ptr) {}
  DeleteTask(std::unique_ptr<T> ptr) : ptr_(std::move(ptr)) {}
  ~DeleteTask() override {}

 private:
  void Execute() override { ptr_.reset(); }

  std::unique_ptr<T> ptr_;

  DISALLOW_COPY_AND_ASSIGN(DeleteTask);
};

// An action that can be cancelled, such as a timer.
class Cancelable {
 public:
  Cancelable();
  virtual ~Cancelable();

  virtual void Cancel() = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(Cancelable);
};

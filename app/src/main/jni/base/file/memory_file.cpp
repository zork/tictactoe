////
// memory_file.cpp
////

#include "base/file/memory_file.h"

#include "base/logging.h"

#include <string.h>
#include <algorithm>

MemoryFile::MemoryFile(bool read_only) : head_(0), read_only_(read_only) {}

MemoryFile::MemoryFile(std::vector<uint8_t>* buffer, bool read_only)
    : head_(0), read_only_(read_only) {
  buffer_.swap(*buffer);
}

MemoryFile::MemoryFile(File* input) : head_(0), read_only_(true) {
  size_t file_size = input->RemainingLength();
  buffer_.resize(file_size);
  size_t bytes_read = input->Read(&buffer_[0], file_size);
  DCHECK_EQ(bytes_read, file_size);
}

MemoryFile::~MemoryFile() {}

size_t MemoryFile::Length() {
  return buffer_.size();
}

size_t MemoryFile::RemainingLength() {
  return buffer_.size() - head_;
}

size_t MemoryFile::Read(void* buffer, size_t bytes) {
  size_t to_read = std::min(buffer_.size() - head_, bytes);
  memcpy(buffer, &buffer_[head_], to_read);
  head_ += to_read;
  return to_read;
}

size_t MemoryFile::Write(const void* buffer, size_t bytes) {
  if (read_only_) {
    NOTREACHED();
    return -1;
  } else {
    if (head_ + bytes > buffer_.size()) {
      buffer_.resize(head_ + bytes);
    }
    memcpy(&buffer_[head_], buffer, bytes);
    head_ += bytes;
    return bytes;
  }
}

bool MemoryFile::Seek(long offset, SeekWhence whence) {
  switch (whence) {
    case File::kSeekSet:
      head_ = offset;
      break;
    case File::kSeekCur:
      head_ += offset;
      break;
    case File::kSeekEnd:
      head_ = buffer_.size() + offset;
      break;
  }
  return head_ == 0 || head_ < buffer_.size();
}

long MemoryFile::Tell() {
  return head_;
}

void MemoryFile::Close() {
  buffer_.clear();
}

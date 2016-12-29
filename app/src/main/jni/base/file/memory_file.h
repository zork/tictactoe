////
// memory_file.h
////

#pragma once

#include "base/file/file.h"

#include <vector>

// File that's stored in memory
class MemoryFile : public File {
 public:
  MemoryFile(bool read_only);
  // Create a MemoryFile from the provided buffer. swaps the data to |buffer_|
  MemoryFile(std::vector<uint8_t>* buffer, bool read_only);
  // Create a MemoryFile from the provided File.
  MemoryFile(File* input);
  ~MemoryFile() override;
  DISALLOW_COPY_AND_ASSIGN(MemoryFile);

  size_t Length() override;
  size_t RemainingLength() override;
  size_t Read(void* buffer, size_t bytes) override;
  size_t Write(const void* buffer, size_t bytes) override;
  bool Seek(long offset, SeekWhence whence) override;
  long Tell() override;
  void Close() override;

  std::vector<uint8_t>* GetBuffer() { return &buffer_; }

 private:
  std::vector<uint8_t> buffer_;
  size_t head_;
  bool read_only_;
};

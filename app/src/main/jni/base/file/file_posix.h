////
// file_posix.h
////

#pragma once

#include "base/file/file.h"

class FilePosix : public File {
 public:
  enum FileMode { kModeNone, kModeRead, kModeWrite, kModeAppend };

  static void Delete(const std::string& path, bool recursive);
  static bool Rename(const std::string& old_filename,
                     const std::string& new_filename);

  FilePosix();
  ~FilePosix() override;

  bool Open(const std::string& filename, FileMode mode);
  size_t Length() override;
  size_t RemainingLength() override;
  int GetFileDescriptor(off_t* start) override;
  size_t Read(void* buffer, size_t bytes) override;
  size_t Write(const void* buffer, size_t bytes) override;
  bool Seek(long offset, SeekWhence whence) override;
  long Tell() override;
  void Close() override;

 private:
  FileMode mode_;
  int fd_;
};

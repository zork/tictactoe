////
// file.h
////

#pragma once

#include "base/basic_types.h"
#include "base/macros.h"

#include <string>

// Interface for reading and writing to files.
class File {
 public:
  enum SeekWhence { kSeekSet, kSeekCur, kSeekEnd };
  File();
  virtual ~File();
  DISALLOW_COPY_AND_ASSIGN(File);

  // Return the number of bytes in the file.
  virtual size_t Length() = 0;

  // Return the number of bytes remaining in the file.
  virtual size_t RemainingLength() = 0;

  // Return a file descriptor for the file.
  // TODO: Perhaps this should be android only.
  virtual int GetFileDescriptor(off_t* start);

  // Read |bytes| bytes into |buffer|.  Returns number of bytes actually read.
  virtual size_t Read(void* buffer, size_t bytes) = 0;

  // Write |bytes| bytes from |buffer|.  Returns number of bytes actually
  // written.
  virtual size_t Write(const void* buffer, size_t bytes);

  // Seek to the specified location.
  // TODO: Remove return value, and assert if seeking before start of file.
  virtual bool Seek(long offset, SeekWhence whence) = 0;

  // Get the current file head.
  virtual long Tell() = 0;

  // Close the file.
  virtual void Close() = 0;

  // Utility functions.

  // Read a string file from the current read position into |output|
  // as a string, reading until a null terminator or EOF.
  // Returns true on succes, false on failure.
  bool ReadString(std::string* output);
  // Read the entire remaining data into |output|
  // Returns true on succes, false on failure.
  bool ReadAsString(std::string* output);
  size_t WriteString(const std::string& value);
  size_t WriteStringWithTerminator(const std::string& value);

  // Read a 4 byte float.
  bool ReadFloat(float* output);
  bool WriteFloat(float value);

  // Read a 4 byte integer.
  bool ReadInt(uint32_t* output);
  bool WriteInt(uint32_t value);

  // Read a 2 byte integer.
  bool ReadShort(uint16_t* output);
  bool WriteShort(uint16_t value);

  // Read a byte integer.
  bool ReadByte(uint8_t* output);
  bool WriteByte(uint8_t value);
};

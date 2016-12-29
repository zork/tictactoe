////
// file.cpp
////

#include "base/file/file.h"

#include "base/logging.h"

#include <memory>

File::File() {}
File::~File() {}

int File::GetFileDescriptor(off_t* start) {
  NOTREACHED();
  return -1;
}

size_t File::Write(const void* buffer, size_t bytes) {
  return -1;
}

bool File::ReadString(std::string* output) {
  uint8_t input;
  while (ReadByte(&input) && input) {
    output->push_back(input);
  }

  return true;
}

bool File::ReadAsString(std::string* output) {
  size_t size = RemainingLength();
  if (!size)
    return false;

  output->resize(size);
  size_t read = Read(&output->at(0), size);

  return read == size;
}

size_t File::WriteString(const std::string& value) {
  return Write(value.c_str(), value.length());
}

size_t File::WriteStringWithTerminator(const std::string& value) {
  if (value.length() != Write(value.c_str(), value.length()))
    return false;
  return WriteByte(0);
}

bool File::ReadFloat(float* output) {
  return Read(output, sizeof(float)) == sizeof(float);
}

bool File::WriteFloat(float value) {
  return Write(&value, sizeof(float)) == sizeof(float);
}

bool File::ReadInt(uint32_t* output) {
  return Read(output, sizeof(uint32_t)) == sizeof(uint32_t);
}

bool File::WriteInt(uint32_t value) {
  return Write(&value, sizeof(uint32_t)) == sizeof(uint32_t);
}

bool File::ReadShort(uint16_t* output) {
  return Read(output, sizeof(uint16_t)) == sizeof(uint16_t);
}

bool File::WriteShort(uint16_t value) {
  return Write(&value, sizeof(uint16_t)) == sizeof(uint16_t);
}

bool File::ReadByte(uint8_t* output) {
  return Read(output, sizeof(uint8_t)) == sizeof(uint8_t);
}

bool File::WriteByte(uint8_t value) {
  return Write(&value, sizeof(uint8_t)) == sizeof(uint8_t);
}

////
// file_manager.cpp
////

#include "base/file/file_manager.h"

#include "base/logging.h"

namespace {
static FileManager* g_file_manager = nullptr;
}

// static
FileManager* FileManager::Get() {
  DCHECK(g_file_manager);
  return g_file_manager;
}

FileManager::FileManager() {
  DCHECK(g_file_manager == nullptr);
  g_file_manager = this;
}

FileManager::~FileManager() {
  DCHECK(g_file_manager == this);
  g_file_manager = nullptr;
}

std::unique_ptr<File> FileManager::OpenAsset(const std::string& filename) {
  return OpenBundledAsset(filename.c_str());
}

std::unique_ptr<File> FileManager::ReadExternalFile(
    const std::string& filename) {
  NOTREACHED();
  return std::unique_ptr<File>(nullptr);
}

std::unique_ptr<File> FileManager::WriteExternalFile(
    const std::string& filename,
    bool append) {
  NOTREACHED();
  return std::unique_ptr<File>(nullptr);
}

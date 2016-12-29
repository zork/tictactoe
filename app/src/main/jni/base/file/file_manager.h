////
// file_manager.h
////

#pragma once

#include "base/file/file.h"
#include "base/macros.h"

#include <memory>
#include <string>
#include <vector>

// The interface for opening files.
class FileManager {
 public:
  static std::unique_ptr<FileManager> Create(const std::string& app_name);
  static std::unique_ptr<FileManager> CreateTestFileManager();
  static FileManager* Get();

  FileManager();
  virtual ~FileManager();
  DISALLOW_COPY_AND_ASSIGN(FileManager);

  // Open a file that is included in the app bundle.  Always read only.
  std::unique_ptr<File> OpenAsset(const std::string& filename);

  // Delete the file at |path|.  If |recursive|, delete all children, too.
  virtual void Delete(const std::string& path, bool recursive) = 0;

  // Rename |old_filename| to |new_filename|
  virtual bool Rename(const std::string& old_filename,
                      const std::string& new_filename) = 0;

  // Read file in the data directory.
  virtual std::unique_ptr<File> ReadFile(const std::string& filename) = 0;

  // Create or append file in the data directory.
  virtual std::unique_ptr<File> WriteFile(const std::string& filename,
                                          bool append) = 0;

  // Read file in an external directory.
  virtual std::unique_ptr<File> ReadExternalFile(const std::string& filename);

  // Create or append file in an external directory.
  virtual std::unique_ptr<File> WriteExternalFile(const std::string& filename,
                                                  bool append);

 private:
  // Open a file that is included in the app bundle.  Always read only.
  virtual std::unique_ptr<File> OpenBundledAsset(const char* filename) = 0;
};

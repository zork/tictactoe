////
// file_manager_android.h
////

#pragma once

#include "base/file/file_manager.h"

#include <jni.h>
#include <string>

class AAssetManager;

class FileManagerAndroid : public FileManager {
 public:
  FileManagerAndroid(jobject asset_manager, const char* data_root);
  ~FileManagerAndroid() override;

 private:
  // FileManager:
  void Delete(const std::string& path, bool recursive) override;
  bool Rename(const std::string& old_filename,
              const std::string& new_filename) override;
  std::unique_ptr<File> ReadFile(const std::string& filename) override;
  std::unique_ptr<File> WriteFile(const std::string& filename,
                                  bool append) override;
  std::unique_ptr<File> OpenBundledAsset(const char* filename) override;

  jobject asset_manager_object_;
  AAssetManager* asset_manager_;
  std::string data_root_;

  DISALLOW_COPY_AND_ASSIGN(FileManagerAndroid);
};

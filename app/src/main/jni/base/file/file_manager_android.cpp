////
// file_manager_android.cpp
////

#include "base/file/file_manager_android.h"

#include "base/file/file.h"
#include "base/file/file_posix.h"
#include "platform/android/android.h"

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <memory>

class FileAndroidAsset : public File {
 public:
  FileAndroidAsset() : asset_(0) {}

  ~FileAndroidAsset() override { Close(); }

  bool OpenAsset(AAssetManager* manager, const char* filename) {
    Close();

    asset_ = AAssetManager_open(manager, filename, AASSET_MODE_UNKNOWN);
    return asset_ != 0;
  }

  size_t Length() override { return AAsset_getLength(asset_); }

  size_t RemainingLength() override {
    if (!asset_)
      return false;

    return AAsset_getRemainingLength(asset_);
  }

  int GetFileDescriptor(off_t* start) override {
    off_t length;
    return AAsset_openFileDescriptor(asset_, start, &length);
  }

  size_t Read(void* buffer, size_t bytes) override {
    return AAsset_read(asset_, buffer, bytes);
  }

  bool Seek(long offset, SeekWhence whence) override {
    int seek_whence;
    switch (whence) {
      case kSeekSet:
        seek_whence = SEEK_SET;
        break;
      case kSeekCur:
        seek_whence = SEEK_CUR;
        break;
      case kSeekEnd:
        seek_whence = SEEK_END;
        break;
    }
    return -1 != AAsset_seek(asset_, offset, seek_whence);
  }

  long Tell() override { return AAsset_seek(asset_, 0, SEEK_CUR); }

  void Close() override {
    if (asset_) {
      AAsset_close(asset_);
      asset_ = 0;
    }
  }

 private:
  AAsset* asset_;
};

FileManagerAndroid::FileManagerAndroid(jobject asset_manager,
                                       const char* data_root)
    : data_root_(data_root) {
  asset_manager_object_ = android::GetJNIEnv()->NewGlobalRef(asset_manager);
  asset_manager_ =
      AAssetManager_fromJava(android::GetJNIEnv(), asset_manager_object_);
}

FileManagerAndroid::~FileManagerAndroid() {
  asset_manager_ = 0;
  android::GetJNIEnv()->DeleteGlobalRef(asset_manager_object_);
}

// private:
// FileManager:
void FileManagerAndroid::Delete(const std::string& path, bool recursive) {
  std::string full_path = data_root_ + path;
  FilePosix::Delete(full_path.c_str(), recursive);
}

bool FileManagerAndroid::Rename(const std::string& old_filename,
                                const std::string& new_filename) {
  std::string old_path = data_root_ + old_filename;
  std::string new_path = data_root_ + new_filename;
  return FilePosix::Rename(old_path, new_path);
}

std::unique_ptr<File> FileManagerAndroid::ReadFile(
    const std::string& filename) {
  std::string full_path = data_root_ + filename;
  std::unique_ptr<FilePosix> file(new FilePosix());
  if (file->Open(full_path, FilePosix::kModeRead)) {
    return std::move(file);
  } else {
    return nullptr;
  }
}

std::unique_ptr<File> FileManagerAndroid::WriteFile(const std::string& filename,
                                                    bool append) {
  std::string full_path = data_root_ + filename;
  std::unique_ptr<FilePosix> file(new FilePosix());
  if (file->Open(full_path,
                 append ? FilePosix::kModeAppend : FilePosix::kModeWrite)) {
    return std::move(file);
  } else {
    return nullptr;
  }
}

std::unique_ptr<File> FileManagerAndroid::OpenBundledAsset(
    const char* filename) {
  std::unique_ptr<FileAndroidAsset> file(new FileAndroidAsset());
  if (file->OpenAsset(asset_manager_, filename)) {
    return std::move(file);
  } else {
    return nullptr;
  }
}

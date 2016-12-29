////
// file_posix.cpp
////

#include "base/file/file_posix.h"

#include "base/logging.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

namespace {

void EnsureDirExists(const std::string& filename) {
  char path[256];
  char* cur = NULL;

  snprintf(path, sizeof(path), "%s", filename.c_str());

  for (cur = path + 1; *cur; ++cur) {
    if (*cur == '/') {
      *cur = 0;
      mkdir(path, S_IRWXU);
      *cur = '/';
    }
  }
}
}

// static
void FilePosix::Delete(const std::string& path, bool recursive) {
  struct stat stats;
  if (!lstat(path.c_str(), &stats)) {
    if (stats.st_mode & S_IFDIR) {
      if (recursive) {
        DIR* dir = opendir(path.c_str());
        if (dir) {
          while (struct dirent* entity = readdir(dir)) {
            // Ignore the current dir and parent.
            if (strcmp(entity->d_name, ".") && strcmp(entity->d_name, "..")) {
              std::string child = path + "/" + entity->d_name;
              Delete(child, true);
            }
          }
          closedir(dir);
        } else {
          LOG(ERROR) << "Could not open dir: " << path;
        }
      }
      if (rmdir(path.c_str())) {
        LOG(ERROR) << "Failed to rmdir " << path << ": " << strerror(errno);
      }
    } else if (stats.st_mode & S_IFREG) {
      if (unlink(path.c_str())) {
        LOG(ERROR) << "Failed to unlink " << path << ": " << strerror(errno);
      }
    }
  }
}

// static
bool FilePosix::Rename(const std::string& old_filename,
                       const std::string& new_filename) {
  int result = rename(old_filename.c_str(), new_filename.c_str());
  if (result) {
    LOG(WARNING) << "Cannot rename " << old_filename << " to " << new_filename
                 << ", errorno: " << strerror(errno);
  }
  return result == 0;
}

FilePosix::FilePosix() : mode_(kModeNone), fd_(-1) {}

FilePosix::~FilePosix() {
  Close();
}

bool FilePosix::Open(const std::string& filename, FileMode mode) {
  Close();

  mode_ = mode;

  int flags = 0;
  mode_t posix_mode = 0;

  switch (mode) {
    case kModeNone:
      LOG(ERROR) << "Open called with kModeNone";
      return false;

    case kModeRead:
      flags = O_RDONLY;
      break;

    case kModeWrite:
      flags = O_WRONLY | O_CREAT | O_TRUNC;
      posix_mode = 0600;
      break;

    case kModeAppend:
      flags = O_WRONLY | O_CREAT | O_APPEND;
      posix_mode = 0600;
      break;
  }

  if (mode == kModeWrite || mode == kModeAppend)
    EnsureDirExists(filename);

  fd_ = open(filename.c_str(), flags, posix_mode);
  return fd_ >= 0;
}

size_t FilePosix::Length() {
  if (fd_ < 0)
    return 0;
  struct stat file_stat;

  if (fstat(fd_, &file_stat) < 0)
    return 0;

  return file_stat.st_size;
}

size_t FilePosix::RemainingLength() {
  if (!fd_)
    return false;

  size_t pos = lseek(fd_, 0, SEEK_SET);
  size_t size = Length();

  return size - pos;
}

int FilePosix::GetFileDescriptor(off_t* start) {
  *start = 0;
  return fd_;
}

size_t FilePosix::Read(void* buffer, size_t bytes) {
  if (mode_ != kModeRead)
    return -1;
  return read(fd_, buffer, bytes);
}

size_t FilePosix::Write(const void* buffer, size_t bytes) {
  if (mode_ != kModeAppend && mode_ != kModeWrite) {
    return -1;
  }
  return write(fd_, buffer, bytes);
}

bool FilePosix::Seek(long offset, SeekWhence whence) {
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
  return lseek(fd_, offset, seek_whence) >= 0;
}

long FilePosix::Tell() {
  return lseek(fd_, 0, SEEK_CUR);
}

void FilePosix::Close() {
  if (fd_ >= 0) {
    close(fd_);
    fd_ = -1;
  }
}

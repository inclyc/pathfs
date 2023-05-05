#include "utils.h"
#include <fcntl.h>

#ifdef linux
// pread(2)
#define _XOPEN_SOURCE 700
#endif

#include <unistd.h>

#define FUSE_USE_VERSION 31
#include <fuse.h>
#include <string>
#include <string_view>

namespace pathfs {

std::string_view envName = "PATH";

static int open(const char *file, struct fuse_file_info *fi) {
  auto path = getPaths(fuse_get_context()->pid, envName);

  for (auto p : path) {
    std::string canoFile = std::string(p) + '/' + std::string(file);
    int fd = ::open(canoFile.c_str(), fi->flags);
    if (fd == -1)
      continue;
    fi->fh = fd;
    return 0;
  }

  return -errno;
};

static int read(const char *file, char *buf, size_t size, off_t offset,
                struct fuse_file_info *fi) {
  int res = ::pread(fi->fh, buf, size, offset);
  if (res == -1)
    return -errno;

  return res;
}

static int readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                   off_t offset, struct fuse_file_info *fi,
                   enum fuse_readdir_flags flags) {

  filler(buf, ".", NULL, 0, FUSE_FILL_DIR_PLUS);
  filler(buf, "..", NULL, 0, FUSE_FILL_DIR_PLUS);
  return 0;
}

static int getattr(const char *path, struct stat *stbuf,
                   struct fuse_file_info *fi) {
  auto paths = getPaths(fuse_get_context()->pid, envName);

  for (auto p : paths) {
    std::string canoFile = std::string(p) + '/' + std::string(path);
    int res = ::stat(canoFile.c_str(), stbuf);
    if (res == -1)
      continue;
    return 0;
  }
  return -errno;
}

static const struct fuse_operations operations = {
    .getattr = getattr,
    .open = open,
    .read = read,
    .readdir = readdir,
};

} // namespace pathfs

int main(int argc, char *argv[]) {
  return fuse_main(argc, argv, &pathfs::operations, NULL);
}

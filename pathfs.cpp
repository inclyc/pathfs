#include "utils.h"
#include <fcntl.h>

#ifdef linux
// pread(2)
#define _XOPEN_SOURCE 700
#endif

#include <cassert>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <unistd.h>

#define FUSE_USE_VERSION 31
#include <fuse.h>

namespace pathfs {

std::string_view envName = "PATH";
static struct options {
  const char *fallback;
  int show_help;
} options;

#define OPTION(t, p)                                                           \
  { t, offsetof(struct options, p), 1 }
static const struct fuse_opt option_spec[] = {
    OPTION("fallback=%s", fallback), OPTION("-h", show_help),
    OPTION("--help", show_help), FUSE_OPT_END};

std::vector<std::string_view>
getPathsWithoutFallback(pid_t pid, std::string_view envName) {
  std::vector<std::string_view> result;
  std::stringstream ss;
  ss << "/proc/" << pid << "/environ";
  std::ifstream ifs(ss.str());
  if (!ifs.is_open())
    return {};

  // read an environment record from procfs.
  // the records is deliminated by '\0', not '\n'
  // proc(5)
  for (std::string record; std::getline(ifs, record, '\0');) {
    auto [name, value] = seperate(record, '=');
    if (name.starts_with(envName)) {
      return split(value, ':');
    }
  }
  return result;
}

std::vector<std::string_view> getPaths(pid_t pid, std::string_view envName) {
  auto ret = getPathsWithoutFallback(pid, envName);
  ret.push_back(options.fallback);
  return ret;
}

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

static void showHelp(std::string_view prog) {
  std::cout << "usage: " << prog << " <ENV> [options] <mountpoint>\n";
  std::cout << "\n";
  std::cout << "Pathfs-specific options:\n";
  std::cout << "     -o fallback=<fallback>     Fallback search PATH\n";
  std::cout << "     -h                         Show help message\n";
  std::cout << "\n";
}

} // namespace pathfs

int main(int argc, char *argv[]) {
  using namespace pathfs;
  if (argc > 2) {
    pathfs::envName = argv[1];
    // Consume argv[1] as "envName" (mount device), feed the reset to
    // "fuse_main"
    for (int i = 1; i < argc - 1; i++) {
      argv[i] = argv[i + 1];
    }
    argc -= 1;
  }

  struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

  /* Set defaults -- we have to use strdup so that
     fuse_opt_parse can free the defaults if other
     values are specified */
  options.fallback = strdup("/ramdom-unused-shelter");

  /* Parse options */
  if (fuse_opt_parse(&args, &options, option_spec, NULL) == -1)
    return 1;

  if (pathfs::options.show_help) {
    showHelp(argv[0]);
    assert(fuse_opt_add_arg(&args, "--help") == 0);
    args.argv[0][0] = '\0';
  }

  int ret = fuse_main(args.argc, args.argv, &pathfs::operations, NULL);
  fuse_opt_free_args(&args);
  return ret;
}

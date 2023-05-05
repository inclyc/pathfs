#include "utils.h"

namespace pathfs {

/// Split a string_view into [ string_view ], seperated by 'sep'
std::vector<std::string_view> split(std::string_view str, char sep) {
  std::vector<std::string_view> result;
  while (!str.empty()) {
    auto npos = std::string_view::npos;
    auto sepLoc = str.find(sep);
    if (sepLoc == npos)
      break;

    result.push_back(str.substr(0, sepLoc));
    str = str.substr(sepLoc + 1, npos);
  }
  if (!str.empty())
    result.push_back(str);
  return result;
}

std::pair<std::string_view, std::string_view> seperate(std::string_view str,
                                                       char sep) {

  auto sepLoc = str.find(sep);

  return {str.substr(0, sepLoc),
          str.substr(std::min(sepLoc + 1, str.length()))};
}

} // namespace pathfs

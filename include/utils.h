#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace pathfs {

std::vector<std::string_view> split(std::string_view, char);

std::pair<std::string_view, std::string_view> seperate(std::string_view, char);

std::vector<std::string_view> getPaths(pid_t pid, std::string_view envName);

} // namespace pathfs

/**
 * @file explorer/namefilter.cpp
 *
 * @brief
 *
 * @author Jordan Bieder
 *
 * @copyright Copyright (C) 2023 Jordan Bieder
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "explorer/namefilter.hpp"

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <regex>

#include "explorer/logger.hpp"

using json = nlohmann::json;

namespace {
constexpr const char kReplacements[] = "replacements";
}

namespace TitleFinder {

namespace Explorer {

NameFilter::NameFilter(std::string_view blacklist) {
  std::filesystem::path check(blacklist);
  if (!std::filesystem::exists(check)) {
    Logger()->error("Blacklist file {} does not seem to exist", blacklist);
  }
  std::ifstream file{check, std::ios::in};
  json db;
  try {
    db = nlohmann::json::parse(file);
  } catch (const std::exception& e) {
    Logger()->critical("json parsing failed with {}", e.what());
    return;
  }
  if (!db.contains(kReplacements)) {
    Logger()->info("Blacklist file does not contain replacements");
    return;
  }
  if (!db[kReplacements].is_array()) {
    Logger()->error("Replacements is not an array");
    return;
  }
  _regex.resize(db[kReplacements].size());
  int i = 0;
  const std::regex protection(R"(([\\\^\$\.\|\?\*\+\(\)\[\]\{\}]))");
  for (const auto& r : db[kReplacements]) {
    std::string source =
        std::regex_replace(r.value("source", ""), protection, "\\$1",
                           std::regex_constants::match_any);
    std::string replacement = r.value("replacement", "");
    bool casesensitive = r["casesensitive"];
    Logger()->trace("source: {: <20s}, replacement {: <20s}", source,
                    replacement);
    _regex[i++] = std::move(
        std::make_pair(std::regex(source, casesensitive ? std::regex::ECMAScript
                                                        : std::regex::icase),
                       replacement));
  }
}

std::string NameFilter::filter(const std::string& input) {
  std::string output(input);
  std::regex trim(R"(\.+)");
  for (const auto& r : _regex) {
    Logger()->trace("Intermediate replacement: {}", output);
    output = std::regex_replace(output, r.first, r.second);
  }
  output =
      std::regex_replace(output, trim, ".", std::regex_constants::match_any);
  return output;
}

} // namespace Explorer

} // namespace TitleFinder

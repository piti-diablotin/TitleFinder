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
#include <memory>
#include <nlohmann/json.hpp>
#include <regex>

#include "explorer/logger.hpp"

using json = nlohmann::json;

namespace {
constexpr const char kReplacements[] = "replacements";
const std::regex kProtection(R"(([\\\^\$\.\|\?\*\+\(\)\[\]\{\}]))");
} // namespace

namespace TitleFinder {

namespace Explorer {

NameFilter::NameFilter(std::string_view blacklist) : _regex(), _db() {
  std::filesystem::path check(blacklist);
  if (!std::filesystem::exists(check)) {
    Logger()->error("Blacklist file {} does not seem to exist", blacklist);
  }
  std::ifstream file{check, std::ios::in};
  if (!file.is_open()) {
    Logger()->error("Blacklist file {} cannot be opened", blacklist);
    return;
  }
  try {
    _db = nlohmann::json::parse(file);
  } catch (const std::exception& e) {
    Logger()->critical("json parsing failed with {}", e.what());
    return;
  }
  if (!_db.contains(kReplacements)) {
    Logger()->info("Blacklist file does not contain replacements");
    return;
  }
  if (!_db[kReplacements].is_array()) {
    Logger()->error("Replacements is not an array");
    return;
  }
  _regex.resize(_db[kReplacements].size());
  int i = 0;
  for (const auto& r : _db[kReplacements]) {
    std::string source =
        std::regex_replace(r.value("source", ""), kProtection, "\\$1",
                           std::regex_constants::match_any);
    if (source.empty())
      continue;
    std::string replacement = r.value("replacement", "");
    bool casesensitive = r["casesensitive"];
    Logger()->trace("source: {: <20s}, replacement {: <20s}", source,
                    replacement);
    _regex[i++] = std::move(
        std::make_pair(std::regex(source, casesensitive ? std::regex::ECMAScript
                                                        : std::regex::icase),
                       replacement));
  }
  _regex.resize(i);
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

void NameFilter::add(const std::string& source, const std::string& replacement,
                     bool casesensitive) {
  if (source.empty())
    return;
  json item = {{"source", source},
               {"replacement", replacement},
               {"casesensitive", casesensitive}};

  if (!_db.contains(kReplacements))
    _db[kReplacements] = json::array();

  _db[kReplacements].push_back(std::move(item));
  std::regex_replace(source, kProtection, "\\$1",
                     std::regex_constants::match_any);
  Logger()->trace("source: {: <20s}, replacement {: <20s}", source,
                  replacement);
  _regex.push_back(std::move(
      std::make_pair(std::regex(source, casesensitive ? std::regex::ECMAScript
                                                      : std::regex::icase),
                     replacement)));
}

void NameFilter::dump(const std::string& filename) {
  std::ofstream output(filename, std::ios::out);
  if (!output.is_open())
    throw std::runtime_error(fmt::format("Unable to open file {}", filename));

  output << _db.dump(2);
  output.close();
}

} // namespace Explorer

} // namespace TitleFinder

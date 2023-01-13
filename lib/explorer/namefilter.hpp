/**
 * @file explorer/namefilter.hpp
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

#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include <regex>
#include <string_view>
#include <utility>
#include <vector>

namespace TitleFinder {

namespace Explorer {

class NameFilter {

public:
  /**
   * Empty constructor
   */
  explicit NameFilter(std::string_view blacklist);

  /**
   * Destructor
   */
  virtual ~NameFilter() = default;

  std::string filter(const std::string& input);

  void add(const std::string& source, const std::string& replacement,
           bool casesensitive);

  void dump(std::string_view filename);

private:
  std::vector<std::pair<std::regex, std::string>> _regex;
  nlohmann::json _db;
};

} // namespace Explorer

} // namespace TitleFinder

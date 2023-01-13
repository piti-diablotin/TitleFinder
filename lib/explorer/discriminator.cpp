/**
 * @file explorer/sort.cpp
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

#include "explorer/discriminator.hpp"

#include <filesystem>
#include <regex>

#include "explorer/logger.hpp"

namespace {

constexpr const char* seasonDiscriminant_{R"([\.\-_][s]?(\d{1,2})$)"};
constexpr const char* episodeDiscriminant_{
    R"([\.\-_][s]?(\d{1,2})[xe](\d{1,2}))"};
constexpr const char* movieDiscriminant_{
    R"([\._\- ]?\(?(\d{4})[^a-zA-Z0-9]\)?)"};

} // namespace

namespace TitleFinder {

namespace Explorer {

Discriminator::Discriminator()
    : _season(-1), _episode(-1), _title(), _year(-1),
      _reS(seasonDiscriminant_, std::regex::icase),
      _reSE(episodeDiscriminant_, std::regex::icase), _reM(movieDiscriminant_) {
}

Type Discriminator::getType(const std::filesystem::path& p) {
  _season = -1;
  _episode = -1;
  _title.clear();
  _year = -1;

  std::string copy = p.stem().string();
  std::smatch m;
  Type t = Type::None;
  if (std::regex_search(copy, m, _reSE)) {
    Logger()->debug("{} analyzed as Show", copy);
    _season = std::stoi(m[1].str());
    _episode = std::stoi(m[2].str());
    _title = m.prefix().str();
    t = Type::Show;
  } else if (std::regex_search(copy, m, _reM)) {
    Logger()->debug("{} analyzed as Movie", copy);
    _title = m.prefix();
    _year = std::stoi(m[1].str());
    t = Type::Movie;
  } else {
    Logger()->debug("{} analyzed as Movie without year", copy);
    t = Type::Movie;
    _title = p.stem();
  }
  return t;
}

} // namespace Explorer

} // namespace TitleFinder

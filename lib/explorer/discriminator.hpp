/**
 * @file explorer/sort.hpp
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

#include <filesystem>
#include <regex>

namespace TitleFinder {

enum class Type { Movie, Show, None };

namespace Explorer {

class Discriminator {

public:
  /**
   * Empty constructor
   */
  Discriminator();

  /**
   * Destructor
   */
  virtual ~Discriminator() = default;

  Type getType(const std::filesystem::path& p);

  inline int getSeason() const { return _season; }

  inline int getEpisode() const { return _episode; }

  inline const std::string& getTitle() const { return _title; }

  inline int getYear() const { return _year; }

private:
  int _season;
  int _episode;
  std::string _title;
  int _year;

  std::regex _reS;
  std::regex _reSE;
  std::regex _reM;
};

} // namespace Explorer

} // namespace TitleFinder

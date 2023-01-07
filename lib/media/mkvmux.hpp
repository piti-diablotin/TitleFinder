/**
 * @file media/mkvmux.hpp
 *
 * @brief
 *
 * @author Jordan Bieder
 *
 * @copyright Copyright (C) 2022 Jordan Bieder
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

#include "media/file.hpp"
#include "media/fileinfo.hpp"

namespace TitleFinder {

namespace Media {

class MkvMux : public File {

public:
  /**
   * Empty constructor
   */
  explicit MkvMux(const FileInfo& input);

  /**
   * Destructor
   */
  virtual ~MkvMux() override = default;

  void transmux(std::string_view output);

  void setTag(int id, std::string value);

private:
  const FileInfo& _input;
};

} // namespace Media

} // namespace TitleFinder

/**
 * @file media/fileinfo.hpp
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

#include <algorithm>
#include <filesystem>
#include <functional>
#include <memory>
#include <string_view>
#include <vector>

#include "media/file.hpp"

extern "C" {
struct AVFormatContext;
struct AVCodecContext;
struct AVCodecParameters;
struct AVCodec;
}

namespace TitleFinder {

namespace Media {

class FileInfo : public File {

public:
  /**
   * Empty constructor
   */
  explicit FileInfo(std::string_view fileuri);

  /**
   * Destructor
   */
  virtual ~FileInfo() override = default;
};

} // namespace Media

} // namespace TitleFinder

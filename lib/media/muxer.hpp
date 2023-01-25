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

class Muxer : public File {

public:
  Muxer() = delete;
  /**
   * Destructor
   */
  virtual ~Muxer() override = default;

  bool transmux(std::string_view output);

  void setTag(int id, std::string value);

protected:
  /**
   * Empty constructor
   */
  explicit Muxer(const FileInfo& input);

  std::string_view _format;
  std::string_view _extension;

private:
  const FileInfo& _input;
};

class MkvMuxer : public Muxer {
public:
  explicit MkvMuxer(const FileInfo& input) : Muxer(input) {
    _format = "matroska";
    _extension = "mkv";
  }
  ~MkvMuxer() override = default;
};

class Mp4Muxer : public Muxer {
public:
  explicit Mp4Muxer(const FileInfo& input) : Muxer(input) {
    _format = "mp4";
    _extension = "mp4";
  }
  ~Mp4Muxer() override = default;
};

class AviMuxer : public Muxer {
public:
  explicit AviMuxer(const FileInfo& input) : Muxer(input) {
    _format = "avi";
    _extension = "avi";
  }
  ~AviMuxer() override = default;
};

} // namespace Media

} // namespace TitleFinder

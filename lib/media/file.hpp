/**
 * @file media/file.hpp
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

#include "media/tags.hpp"

extern "C" {
struct AVFormatContext;
struct AVCodecContext;
}

namespace TitleFinder {

namespace Media {

class File {

public:
  enum class ACodec { Mp3, Aac, Ac3, Other };
  enum class VCodec { H264, Hevc, Av1, Mpeg4, Other };
  enum class Container { Mkv, Avi, Mp4, Other };
  using Lang = std::string;

  /**
   * Destructor
   */
  virtual ~File() = default;

  virtual VCodec getVideoCodec() const;

  virtual ACodec getAudioCodec() const;

  virtual const std::vector<Lang>& getLanguages() const;

  virtual const std::vector<Lang>& getSubtitles() const;

  virtual std::filesystem::path getPath() const;

  virtual const std::string_view getTag(int id) const;

  virtual void dumpInfo() const;

protected:
  /**
   * Empty constructor
   */
  explicit File(std::string_view fileuri);

  std::filesystem::path _path;
  std::vector<Lang> _languages;
  std::vector<Lang> _subtitles;
  VCodec _videoCodec;
  ACodec _audioCodec;
  Container _container;

  std::unique_ptr<AVFormatContext, std::function<void(AVFormatContext*)>>
      _formatCtxt;
  std::unique_ptr<AVCodecContext, std::function<void(AVCodecContext*)>>
      _codecCtxt;

  Tag::TagDict _tags;

  friend class MkvMux;
};

} // namespace Media

} // namespace TitleFinder

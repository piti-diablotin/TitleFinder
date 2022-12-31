/**
 * @file media/file.cpp
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

#include "media/mkvmux.hpp"
#include <filesystem>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/error.h>
}

#include "media/logger.hpp"

namespace TitleFinder {

namespace Media {

MkvMux::MkvMux(const FileInfo& input) : File(""), _input(input) {
  // Open file
  Logger()->debug("Transmuxer MKV of {}", _input.getPath().string());
  _videoCodec = _input.getVideoCodec();
  _audioCodec = _input.getAudioCodec();
  _languages = _input.getLanguages();
  _subtitles = _input.getSubtitles();
  _container = Container::Mkv;
}

void MkvMux::transmux(std::string_view output) {
  _path = output;
  if (std::filesystem::exists(_path)) {
    Logger()->error("Output file {} already exists.", _path.string());
    return;
  }
  AVFormatContext* fc = nullptr;
  if (avformat_open_input(&fc, _path.c_str(), NULL, NULL) != 0) {
    Logger()->error("Unable to open output file {}", _path.string());
    return;
  }
  _formatCtxt.reset(fc);
}

} // namespace Media

} // namespace TitleFinder

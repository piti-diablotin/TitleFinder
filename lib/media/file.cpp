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

#include "media/file.hpp"
#include <string_view>
#include <vector>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/error.h>
}

#include "media/logger.hpp"

namespace TitleFinder {

namespace Media {

File::File(std::string_view fileuri)
    : _path(fileuri), _languages(), _subtitles(), _videoCodec(VCodec::Other),
      _audioCodec(ACodec::Other),
      _container(Container::Other), _formatCtxt{nullptr,
                                                [](AVFormatContext* ctxt) {
                                                  if (ctxt != nullptr) {
                                                    Logger()->debug(
                                                        "Free AVFormatContext");
                                                    avformat_close_input(&ctxt);
                                                    avformat_free_context(ctxt);
                                                  }
                                                }},
      _codecCtxt{nullptr,
                 [](AVCodecContext* ctxt) {
                   if (ctxt != nullptr) {
                     Logger()->debug("Free AVCodecContext");
                     avcodec_free_context(&ctxt);
                   }
                 }},
      _tags() {}

File::VCodec File::getVideoCodec() const { return _videoCodec; }

File::ACodec File::getAudioCodec() const { return _audioCodec; }

const std::vector<File::Lang>& File::getLanguages() const { return _languages; }

const std::vector<File::Lang>& File::getSubtitles() const { return _subtitles; }

std::filesystem::path File::getPath() const { return _path; }

const std::string_view File::getTag(const int id) const { return _tags.at(id); }

void File::dumpInfo() const {
  if (!_formatCtxt) {
    Logger()->warn("Format context not allocated for {}.", _path.string());
    return;
  }
  Logger()->info("{}: {}", _path.string(),
                 _formatCtxt->iformat ? _formatCtxt->iformat->name
                                      : _formatCtxt->oformat->name);
  if (!_tags.empty()) {
    Logger()->info("  Metadata:");
    for (auto& meta : _tags) {
      Logger()->info("    {: <12s}: {}", Tag::tags[meta.first], meta.second);
    }
  }
  if (!_languages.empty()) {
    Logger()->info("  Audio:");
    for (auto& l : _languages)
      Logger()->info("    {}", l);
  }
  if (!_subtitles.empty()) {
    Logger()->info("  Subtitles:");
    for (auto& s : _subtitles)
      Logger()->info("    {}", s);
  }
}

} // namespace Media

} // namespace TitleFinder

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

File::File(const std::string_view fileuri)
    : _path(fileuri), _languages(), _videoCodec(VCodec::Other),
      _audioCodec(ACodec::Other),
      _container(Container::Other), _formatCtxt{nullptr,
                                                [](AVFormatContext* ctxt) {
                                                  if (ctxt != nullptr) {
                                                    avformat_close_input(&ctxt);
                                                    avformat_free_context(ctxt);
                                                  }
                                                }},
      _codecCtxt{nullptr, [](AVCodecContext* ctxt) {
                   if (ctxt != nullptr)
                     avcodec_free_context(&ctxt);
                 }} {
  // Open file
  AVFormatContext* fc = nullptr;
  if (avformat_open_input(&fc, _path.c_str(), NULL, NULL) != 0) {
    Logger()->error("Unable to open input file {}", _path.string());
    return;
  }
  _formatCtxt.reset(fc);
  if (avformat_find_stream_info(fc, NULL) != 0) {
    Logger()->error("Unable to find stream info for file {}", _path.string());
  }
  Logger()->debug("Format name is {} ({})", fc->iformat->name, _path.string());
  std::string_view demuxer{fc->iformat->name};
  if (demuxer.find("mastroska") != std::string_view::npos) {
    _container = Container::Mkv;
  } else if (demuxer.find("mp4") != std::string_view::npos) {
    _container = Container::Mp4;
  } else if (demuxer.find("avi") != std::string_view::npos) {
    _container = Container::Avi;
  } else {
    _container = Container::Other;
  }

  AVDictionaryEntry* mediaTitle =
      av_dict_get(fc->metadata, "title", nullptr, 0);
  if (mediaTitle != nullptr) {
    Logger()->debug("Title: {}", mediaTitle->value);
  }

  for (unsigned i = 0; i < fc->nb_streams; ++i) {
    AVCodecParameters* codecParams = fc->streams[i]->codecpar;
    const AVCodecDescriptor* codecDesc =
        avcodec_descriptor_get(codecParams->codec_id);
    std::string_view name;
    if (codecDesc != nullptr)
      name = codecDesc->name;
    Logger()->debug("Stream [{}] is {}", i, name);
    AVDictionary* meta = fc->streams[i]->metadata;
    AVDictionaryEntry* title = av_dict_get(meta, "title", nullptr, 0);
    if (title != nullptr) {
      Logger()->debug("  title: {}", title->value);
    }
    if (codecParams->codec_type == AVMEDIA_TYPE_AUDIO) {
      if (name == "aac")
        _audioCodec = ACodec::Aac;
      else if (name == "ac3")
        _audioCodec = ACodec::Ac3;
      else if (name == "mp3")
        _audioCodec = ACodec::Mp3;
      else
        _audioCodec = ACodec::Other;
      AVDictionaryEntry* lang = av_dict_get(meta, "language", nullptr, 0);
      if (lang)
        Logger()->debug("  language:{}", lang->value);
    } else if (codecParams->codec_type == AVMEDIA_TYPE_VIDEO) {
      if (name == "av1")
        _videoCodec = VCodec::Av1;
      else if (name == "h264")
        _videoCodec = VCodec::H264;
      else if (name == "hevc")
        _videoCodec = VCodec::Hevc;
      else if (name == "mpeg4")
        _videoCodec = VCodec::Mpeg4;
      else
        _audioCodec = ACodec::Other;
    }
  }
}

File::VCodec File::getVideoCodec() const { return _videoCodec; }

File::ACodec File::getAudioCodec() const { return _audioCodec; }

const std::vector<File::Lang>& File::getLanguages() const { return _languages; }

std::filesystem::path File::getPath() const { return _path; }

} // namespace Media

} // namespace TitleFinder

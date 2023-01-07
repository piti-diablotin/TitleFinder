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

#include "media/fileinfo.hpp"
#include <string_view>
#include <vector>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/error.h>
}

#include "media/logger.hpp"
#include "media/tags.hpp"

namespace TitleFinder {

namespace Media {

FileInfo::FileInfo(const std::string_view fileuri) : File(fileuri) {
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
  Logger()->debug("{}: {}", _path.string(), fc->iformat->name);
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

  AVDictionaryEntry* metainfo = nullptr;
  while ((metainfo =
              av_dict_get(fc->metadata, "", metainfo, AV_DICT_IGNORE_SUFFIX))) {
    const int id = Tag::operator""_tagid(metainfo->key, strlen(metainfo->key));
    if (id >= 0) {
      _tags[id] = metainfo->value;
      Logger()->debug("  {: <12s}: {}", metainfo->key, metainfo->value);
    }
  }

  for (unsigned i = 0; i < fc->nb_streams; ++i) {
    AVCodecParameters* codecParams = fc->streams[i]->codecpar;
    const AVCodecDescriptor* codecDesc =
        avcodec_descriptor_get(codecParams->codec_id);
    std::string_view name;
    if (codecDesc != nullptr)
      name = codecDesc->name;
    Logger()->debug("- Stream [{}] :  {}", i, name);
    AVDictionary* meta = fc->streams[i]->metadata;
    AVDictionaryEntry* title = av_dict_get(meta, "title", nullptr, 0);
    if (title != nullptr) {
      Logger()->debug("    title: {}", title->value);
    }
    AVDictionaryEntry* dictEntry = nullptr;
    switch (codecParams->codec_type) {
    case AVMEDIA_TYPE_AUDIO:
      switch (codecParams->codec_id) {
      case AV_CODEC_ID_AAC:
        _audioCodec = ACodec::Aac;
        break;
      case AV_CODEC_ID_AC3:
        _audioCodec = ACodec::Ac3;
        break;
      case AV_CODEC_ID_MP3:
        _audioCodec = ACodec::Mp3;
        break;
      default:
        _audioCodec = ACodec::Other;
        break;
      }
      dictEntry = av_dict_get(meta, "language", nullptr, 0);
      if (dictEntry) {
        Logger()->debug("    language : {}", dictEntry->value);
        _languages.push_back(std::string(dictEntry->value));
      }

      break;
    case AVMEDIA_TYPE_VIDEO:
      switch (codecParams->codec_id) {
      case AV_CODEC_ID_AV1:
        _videoCodec = VCodec::Av1;
        break;
      case AV_CODEC_ID_H264:
        _videoCodec = VCodec::H264;
        break;
      case AV_CODEC_ID_H265:
        _videoCodec = VCodec::Hevc;
        break;
      case AV_CODEC_ID_MPEG4:
        _videoCodec = VCodec::Mpeg4;
        break;
      default:
        _audioCodec = ACodec::Other;
        break;
      }
      break;
    case AVMEDIA_TYPE_SUBTITLE:
      dictEntry = av_dict_get(meta, "language", nullptr, 0);
      if (dictEntry) {
        Logger()->debug("  subtitles : {}", dictEntry->value);
        _subtitles.push_back(std::string(dictEntry->value));
      }
      break;
    default:
      continue;
    }
  }
}

} // namespace Media

} // namespace TitleFinder

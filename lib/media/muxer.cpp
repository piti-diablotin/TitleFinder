/**
 * @file media/muxer.cpp
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

#include "media/muxer.hpp"

#include <filesystem>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/error.h>
#include <libavutil/mathematics.h>
}

#include "media/logger.hpp"
#include "media/tags.hpp"

namespace TitleFinder {

namespace Media {

Muxer::Muxer(const FileInfo& input)
    : File(""), _format(), _extension(""), _input(input) {
  // Open file
  _videoCodec = _input.getVideoCodec();
  _audioCodec = _input.getAudioCodec();
  _languages = _input.getLanguages();
  _subtitles = _input.getSubtitles();
  _container = Container::Mkv;
  _tags = _input._tags;
}

bool Muxer::transmux(std::string_view output) {
  _path = output;
  _path.replace_extension(_extension);
  Logger()->info("{} transmuxing {} to {}", _format, _input.getPath().string(),
                 _path.string());

  if (!_input.isOpen()) {
    Logger()->error("Input file {} is not opened.", _input._path.string());
    return false;
  }

  int ret = 0;
  if (std::filesystem::exists(_path)) {
    Logger()->error("Output file {} already exists.", _path.string());
    return false;
  }

  AVFormatContext* output_fc = nullptr;
  avformat_alloc_output_context2(&output_fc, nullptr, &_format[0],
                                 _path.c_str());
  if (!output_fc) {
    Logger()->error("Unable to create output context for file {}",
                    _path.string());
    return false;
  }
  _formatCtxt.reset(output_fc);

  AVFormatContext* input_fc = nullptr;
  input_fc = _input._formatCtxt.get();

  if (input_fc->metadata) {
    Logger()->debug("Copy metadata from file {}", _input.getPath().string());
    ret = av_dict_copy(&output_fc->metadata, input_fc->metadata, 0);
    if (ret < 0) {
      Logger()->warn("Failed to copy metadata from file {}",
                     _input.getPath().string());
    }
  }

  if (!_tags.empty()) {
    Logger()->debug("Setting new tag values");
    for (auto& t : _tags) {
      Logger()->trace("Set tag {} to {}", Tag::tags[t.first], t.second);
      ret = av_dict_set(&output_fc->metadata, Tag::tags[t.first],
                        t.second.c_str(), 0);
      if (ret < 0)
        Logger()->warn("Can not set tag {} to {}", Tag::tags[t.first],
                       t.second);
    }
  }

  auto nbStreams = input_fc->nb_streams;

  Logger()->debug("Allocate streams");
  for (unsigned int is = 0; is < nbStreams; ++is) {
    AVStream* outStream;
    AVStream* inStream = input_fc->streams[is];

    outStream = avformat_new_stream(output_fc, nullptr);
    if (!outStream) {
      Logger()->error("Allocation of stream {} failed.", is);
      return false;
    }

    AVCodecParameters* params = outStream->codecpar;
    auto codecTag = params->codec_tag;

    ret = avcodec_parameters_copy(outStream->codecpar, inStream->codecpar);
    if (ret < 0) {
      Logger()->error("Unable to copy codec parametersfor stream {}.", is);
      return false;
    }
    if (!codecTag) {
      unsigned int codecTagTmp;
      if (!output_fc->oformat->codec_tag ||
          av_codec_get_id(output_fc->oformat->codec_tag, params->codec_tag) ==
              params->codec_id ||
          !av_codec_get_tag2(output_fc->oformat->codec_tag, params->codec_id,
                             &codecTagTmp))
        codecTag = params->codec_tag;
    }
    params->codec_tag = codecTag;
    outStream->avg_frame_rate = inStream->avg_frame_rate;
    ret = avformat_transfer_internal_stream_timing_info(
        output_fc->oformat, outStream, inStream,
        AVTimebaseSource::AVFMT_TBCF_AUTO);
    if (ret < 0) {
      Logger()->error("Unable to transfer timing info for stream {}.", is);
      return false;
    }

    if (outStream->time_base.num <= 0 || outStream->time_base.den <= 0) {
      Logger()->debug("Setting time base info for stream {}.", is);
      outStream->time_base =
          av_add_q(av_stream_get_codec_timebase(outStream), {0, 1});
    }
    if (outStream->duration <= 0 && inStream->duration > 0) {
      Logger()->debug("Setting duration info for stream {}.", is);
      outStream->duration = av_rescale_q(
          inStream->duration, inStream->time_base, outStream->time_base);
    }

    if (inStream->metadata) {
      Logger()->debug("Copy metadata for stream {}.", is);
      ret = av_dict_copy(&outStream->metadata, inStream->metadata, 0);
      if (ret < 0) {
        Logger()->warn("Error copying metadata for stream {}.", is);
      }
    }

    if (inStream->nb_side_data) {
      Logger()->debug("Copy side data for stream {}.", is);
      for (int i = 0; i < inStream->nb_side_data; i++) {
        Logger()->trace("side data {}.", i);
        const AVPacketSideData* sdSrc = &inStream->side_data[i];
        uint8_t* dstData;

        dstData = av_stream_new_side_data(outStream, sdSrc->type, sdSrc->size);
        if (!dstData) {
          Logger()->warn("Can not allocate side data for stream {}.", is);
        } else {
          std::copy_n(sdSrc->data, sdSrc->size, dstData);
        }
      }
    }

    if (params->codec_type == AVMEDIA_TYPE_AUDIO) {
      params->frame_size = inStream->codecpar->frame_size;
      if ((params->block_align == 1 || params->block_align == 1152 ||
           params->block_align == 576) &&
          params->codec_id == AV_CODEC_ID_MP3)
        params->block_align = 0;
      if (params->codec_id == AV_CODEC_ID_AC3)
        params->block_align = 0;
    }
  }

  // av_dump_format(_input._formatCtxt.get(), 0, _input.getPath().c_str(), 0);
  // av_dump_format(output_fc, 0, _path.c_str(), 1);

  Logger()->debug("Open output file");
  if (!(output_fc->oformat->flags & AVFMT_NOFILE)) {
    ret = avio_open(&output_fc->pb, _path.c_str(), AVIO_FLAG_WRITE);
    if (ret < 0) {
      Logger()->error("Could not open output file {}", _path.string());
      return false;
    }
  } else {
    Logger()->critical("Context does not have a file output", _path.string());
    return false;
  }

  Logger()->debug("Write header");
  ret = avformat_write_header(output_fc, nullptr);
  if (ret < 0) {
    Logger()->critical("Could not write header to file {}", _path.string());
    return false;
  }

  AVPacket* packet = av_packet_alloc();
  Logger()->debug("Copy streams");
  while (1) {
    AVStream *inStream, *outStream;
    ret = av_read_frame(input_fc, packet);
    if (ret < 0) {
      break;
    }
    inStream = input_fc->streams[packet->stream_index];
    outStream = output_fc->streams[packet->stream_index];
    /* copy packet */
    packet->pts = av_rescale_q_rnd(
        packet->pts, inStream->time_base, outStream->time_base,
        static_cast<AVRounding>(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
    packet->dts = av_rescale_q_rnd(
        packet->dts, inStream->time_base, outStream->time_base,
        static_cast<AVRounding>(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
    if (packet->pts == AV_NOPTS_VALUE && packet->dts != AV_NOPTS_VALUE)
      packet->pts = packet->dts;
    else if (packet->dts == AV_NOPTS_VALUE && packet->pts != AV_NOPTS_VALUE)
      packet->dts = packet->pts;
    packet->duration = av_rescale_q(packet->duration, inStream->time_base,
                                    outStream->time_base);
    packet->pos = -1;

    ret = av_interleaved_write_frame(output_fc, packet);
    if (ret < 0) {
      Logger()->error("Error writting packet");
      break;
    }
    av_packet_unref(packet);
  }

  Logger()->debug("Write trailer");
  av_write_trailer(output_fc);
  return true;
}

void Muxer::setTag(const int id, std::string value) {
  if (id >= 0 && id < Tag::numberOfTags)
    _tags.insert_or_assign(id, std::move(value));
  else
    Logger()->warn("Tag id {} does not exist", id);
}

} // namespace Media

} // namespace TitleFinder

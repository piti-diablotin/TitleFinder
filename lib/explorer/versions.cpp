/**
 * @file explorer/versions.cpp
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

#include "explorer/versions.hpp"

#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <spdlog/version.h>
#include <string_view>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/error.h>
}

namespace TitleFinder {

namespace Explorer {

std::string_view version() { return TITLEFINDER_VERSION; }

std::string allVersions() {
  return fmt::format(
      "{: <15}: {}.{}.{}\n"
      "{: <15}: {}.{}.{}\n"
      "{: <15}: {}.{}.{}\n"
      "{: <15}: {}.{}.{}\n"
      "{: <15}: {}.{}.{}\n"
      "{: <15}: {} ({})",
      "libavformat", AV_VERSION_MAJOR(avformat_version()),
      AV_VERSION_MINOR(avformat_version()),
      AV_VERSION_MICRO(avformat_version()), "libcodec",
      AV_VERSION_MAJOR(avcodec_version()), AV_VERSION_MINOR(avcodec_version()),
      AV_VERSION_MICRO(avcodec_version()), "libavutil",
      AV_VERSION_MAJOR(avutil_version()), AV_VERSION_MINOR(avutil_version()),
      AV_VERSION_MICRO(avutil_version()), "spdlog", SPDLOG_VER_MAJOR,
      SPDLOG_VER_MINOR, SPDLOG_VER_PATCH, "nlohmann_json",
      NLOHMANN_JSON_VERSION_MAJOR, NLOHMANN_JSON_VERSION_MINOR,
      NLOHMANN_JSON_VERSION_PATCH, "tilefinder", TITLEFINDER_VERSION, __DATE__);
}

std::string_view buildDate() { return __DATE__; }

} // namespace Explorer

} // namespace TitleFinder

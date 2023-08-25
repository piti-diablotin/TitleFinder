/**
 * @file logger/logger.cpp
 *
 * @brief Comfigure logger
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

#include "logger/logger.hpp"
#include <cstdlib>
#include <memory>
#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <string_view>

#ifdef __GNUC__
#  if __GNUC__ >= 4
#    if __GNUC_MINOR__ >= 6
#      pragma GCC diagnostic push
#      pragma GCC diagnostic ignored "-Weffc++"
#    endif
#  endif
#endif
#include "spdlog/sinks/ansicolor_sink.h"
#ifdef __GNUC__
#  if __GNUC__ >= 4
#    if __GNUC_MINOR__ >= 6
#      pragma GCC diagnostic pop
#    endif
#  endif
#endif

namespace {

std::mutex _m;

std::shared_ptr<spdlog::logger> buildLogger(const std::string& name) {
  std::unique_lock lk(_m);
  if (auto logger = spdlog::get(name))
    return logger;
  auto output_console_sink =
      std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();

  //  auto output_file_sink =
  //  std::make_shared<spdlog::sinks::basic_file_sink_mt>(
  //      "titlefinder_api.log", false);

  auto log = new spdlog::logger(name, {output_console_sink});
  log->set_pattern(R"([TitleFinder::%n] -%t- %^%v%$)");

#ifndef NDEBUG
  log->set_level(spdlog::level::debug);
#else
  log->set_level(spdlog::level::warn);
#endif
  std::shared_ptr<spdlog::logger> logger{log};
  spdlog::register_logger(logger);

  char* level = ::getenv("TITLEFINDER_LEVEL");
  if (level) {
    if (::strcmp(level, "trace") == 0) {
      log->set_level(spdlog::level::trace);
    } else if (::strcmp(level, "debug") == 0) {
      log->set_level(spdlog::level::debug);
    } else if (::strcmp(level, "info") == 0) {
      log->set_level(spdlog::level::info);
    } else if (::strcmp(level, "warn") == 0) {
      log->set_level(spdlog::level::warn);
    } else if (::strcmp(level, "error") == 0) {
      log->set_level(spdlog::level::err);
    } else if (::strcmp(level, "critical") == 0) {
      log->set_level(spdlog::level::critical);
    } else
      log->warn("Level {} not recognized", level);
  }

  log->debug("Logger level to {}", static_cast<int>(log->level()));
  return logger;
}
} // namespace

namespace TitleFinder {

namespace Logger {

std::shared_ptr<spdlog::logger> getLogger(const std::string& name) {
  auto log = spdlog::get(name);
  return (!log) ? buildLogger(name) : log;
}

} // namespace Logger

} // namespace TitleFinder

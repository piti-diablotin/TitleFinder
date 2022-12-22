/**
 * @file api/logger.hpp
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

#include <memory>
#include <mutex>
#ifdef __GNUC__
#if __GNUC__ >= 4
#if __GNUC_MINOR__ >= 6
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#endif
#endif
#pragma GCC system_header
#endif
#include "spdlog/spdlog.h"
#ifdef __GNUC__
#if __GNUC__ >= 4
#if __GNUC_MINOR__ >= 6
#pragma GCC diagnostic pop
#endif
#endif
#endif

namespace TitleFinder {

namespace Api {

class Logger {

private:
  static std::shared_ptr<spdlog::logger> _logger; ///< Main logger to use
  static std::mutex _m;

  /**
   * Empty constructor
   */
  Logger() {}

  /**
   * Build the logger with all the default things
   */
  static void buildLogger();

public:
  /**
   * Get the default logger
   * @return The logger to use inside epegen
   */
  static std::shared_ptr<spdlog::logger> getLogger();
};

constexpr auto Logger = Api::Logger::getLogger;

} // namespace Api

} // namespace TitleFinder

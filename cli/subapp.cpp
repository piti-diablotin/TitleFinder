/**
 * @file cli/subapp.cpp
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

#include "subapp.hpp"

#include <fmt/ostream.h>
#include <iostream>

namespace TitleFinder {

namespace Cli {

SubApp::SubApp(int argc, char* argv[]) : Application(argc, argv), _engine() {
  _parser.setOption("api_key", 'k', "", "Api key for TheMovieDB");
  _parser.setOption("language", 'l', "en-US",
                    "ISO-639-1 language code (e.g. fr-FR)");
}

int SubApp::readyEngine() {
  try {
    std::string key = _parser.getOption<std::string>("api_key");
    _engine.setTmdbKey(key);
  } catch (const std::exception& e) {
    fmt::print(std::cerr, "Exception occured: {}\n", e.what());
    return 1;
  }

  if (_parser.isSetOption("language")) {
    _engine.setLanguage(_parser.getOption<std::string>("language"));
  }
  return 0;
}

} // namespace Cli

} // namespace TitleFinder

/**
 * @file cli/none.cpp
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

#include "none.hpp"
#include <fmt/format.h>
#include <iostream>

namespace {
std::string getVersion() {
  return fmt::format("{} version {}", TITLEFINDER_NAME, TITLEFINDER_VERSION);
}
} // namespace

namespace TitleFinder {

namespace Cli {

None::None(int argc, char* argv[]) : Application(argc, argv) {
  _parser.setOption("version", 'v', "Print help message");
  _parser.setBinaryName(TITLEFINDER_NAME " (search|rename|scan)");
}

int None::run() {
  try {
    _parser.parse();
  } catch (const std::exception& e) {
    std::cerr << "Exception occured: " << e.what() << std::endl;
    return 1;
  }

  if (_parser.isSetOption("help")) {
    std::cout << _parser << std::endl;
  }
  if (_parser.isSetOption("version")) {
    std::cout << getVersion() << std::endl;
  }
  return 0;
}

} // namespace Cli

} // namespace TitleFinder

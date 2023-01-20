/**
 * @file cli/scan.cpp
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

#include "scan.hpp"
#include <filesystem>
#include <fmt/core.h>
#include <fmt/ostream.h>

namespace TitleFinder {

namespace Cli {

Scan::Scan(int argc, char* argv[]) : Rename(argc, argv) {
  _parser.setBinaryName(TITLEFINDER_NAME " scan");
}

void Scan::setOptionalOptions() {
  _parser.setOption("interactiverun", 'i',
                    "Ask to confirm before applying modifications.");
  _parser.setOption("jobs", 'j', "1",
                    "Number of jobs working at the same time (only in none "
                    "interactive mode).");
  _parser.setOption("recursive", 'r', "Scan files recursively");
}

int Scan::run() {
  if (this->prepare()) {
    return 1;
  }

  if (!std::filesystem::is_directory(_filename)) {
    fmt::print(std::cerr, "{} is not a directory.\n", _filename);
    return 1;
  }

  if (this->readyEngine()) {
    return 1;
  }

  auto list = _engine.listFiles(_filename, _parser.isSetOption("recursive"));
  fmt::print("Will analyze {} files", list.size());
  if (_parser.isSetOption("interactive")) {
    while (!list.empty()) {
      try {
        auto file = list.front();
        auto pred = _engine.predictFile(file, _container, _outputDirectory);
        this->print(pred);
        const std::string_view yes("y");
        const std::string_view no("n");
        std::string rep;
        do {
          fmt::print("Apply ?[{}/{}] ", yes, no);
          std::cin >> rep;
        } while (rep != yes && rep != no);
        if (rep == yes) {
          _engine.apply(pred);
        }
      } catch (const std::exception& e) {
        fmt::print(std::cerr, "Exception occured: {}", e.what());
      }
    }
  } else {
    int jobs = 1;
    try {
      jobs = _parser.getOption<int>("jobs");
    } catch (const std::exception& e) {
      jobs = 1;
    }
    _engine.autoRename(list, _container, jobs, _outputDirectory);
  }
  return 0;
}

} // namespace Cli

} // namespace TitleFinder

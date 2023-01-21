/**
 * @file cli/rename.cpp
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

#include "rename.hpp"

#include <filesystem>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <stdexcept>

#include "explorer/engine.hpp"
#include "media/fileinfo.hpp"

namespace TitleFinder {

namespace Cli {

Rename::Rename(int argc, char* argv[])
    : SubApp(argc, argv), _filename(argv[argc - 1]), _outputDirectory(),
      _container(Media::FileInfo::Container::Other), _engine() {
  _parser.setBinaryName(TITLEFINDER_NAME " rename");
  _parser.setOption("muxer", 'm', "", "Output container", {"mkv", "mp4"});
  _parser.setOption("output-directory", 'o', "",
                    "Output directory to rename/remux the file.");
  _parser.setOption("blacklist", 'b', "", "Blacklist file containing filters");
  if (argc - 1 >= 0)
    _filename = argv[argc - 1];
}

void Rename::setOptionalOptions() {
  _parser.setOption("dry-run", 'd',
                    "Dry run, do not apply change, just print suggestion");
}

int Rename::prepare() {
  this->setOptionalOptions();
  try {
    _parser.parse();

    if (_parser.isSetOption("help")) {
      std::cout << _parser << std::endl;
      return 1;
    }

    _outputDirectory = std::filesystem::absolute(_filename).parent_path();
    if (_parser.isSetOption("output-directory")) {
      _outputDirectory = _parser.getOption<std::string>("output-directory");
    }
    _outputDirectory = std::filesystem::canonical(_outputDirectory);

    if (!std::filesystem::is_directory(_outputDirectory)) {
      throw std::runtime_error(
          fmt::format("{} is not a directory", _outputDirectory.string()));
    }

    _container = Media::FileInfo::Container::Other;
    if (_parser.getOption<std::string>("muxer") == "mp4")
      _container = Media::FileInfo::Container::Mp4;
    else if (_parser.getOption<std::string>("muxer") == "mkv")
      _container = Media::FileInfo::Container::Mkv;

  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  return 0;
}

int Rename::readyEngine() {
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

  if (_parser.isSetOption("blacklist")) {
    _engine.setBlacklist(_parser.getOption<std::string>("blacklist"));
  }
  return 0;
}

int Rename::run() {

  if (this->prepare()) {
    return 1;
  }

  if (!std::filesystem::is_regular_file(_filename)) {
    fmt::print(std::cerr, "{} is not a regular file.\n", _filename);
    return 1;
  }

  if (this->readyEngine()) {
    return 1;
  }

  try {
    auto prediction =
        _engine.predictFile(_filename, _container, _outputDirectory);
    this->print(prediction);
    if (_parser.isSetOption("dry-run"))
      return 0;

    try {
      return _engine.apply(prediction);
    } catch (const std::exception& e) {
      fmt::print(std::cerr, "Failed to apply prediction on file {}: {}\n",
                 _filename, e.what());
      return 1;
    }
  } catch (const std::exception& e) {
    fmt::print(std::cerr, "Failed to predict file {}: {}\n", _filename,
               e.what());
    return 1;
  }
}

void Rename::print(const Explorer::Engine::Prediction& prediction) {
  // prediction.input.dumpInfo();
  if (prediction.movie) {
    fmt::print("Input file => {}\n", prediction.input.getPath().string());
    fmt::print("{} ({})\n", prediction.movie->title,
               prediction.movie->release_date);
    fmt::print("{}\n", prediction.movie->overview);
    fmt::print("Rating: {:2.1f}\n", prediction.movie->vote_average);
    fmt::print("Output file => {}\n", prediction.output);
  } else if (prediction.tvshow && prediction.episode) {
    fmt::print("{} {:d}x{:02d} {} ({})\n", prediction.tvshow->name,
               prediction.episode->season_number,
               prediction.episode->episode_number, prediction.episode->name,
               prediction.episode->air_date);
    fmt::print("{}\n", prediction.episode->overview);
    fmt::print("Rating: {:2.1f}\n", prediction.episode->vote_average);
    fmt::print("Output file => {}\n", prediction.output);
  }
}

} // namespace Cli

} // namespace TitleFinder

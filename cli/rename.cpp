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
#include <stdexcept>

#include "explorer/engine.hpp"
#include "media/fileinfo.hpp"

namespace TitleFinder {

namespace Cli {

Rename::Rename(int argc, char* argv[])
    : Application(argc, argv), _filename(argv[argc - 1]) {
  _parser.setBinaryName("titlefinder_cli rename");
  _parser.setOption("dry-run", 'd',
                    "Dry run, do not apply change, just print suggestion");
  _parser.setOption("muxer", 'm', "mkv", "Output container", {"mkv", "mp4"});
  _parser.setOption("output-directory", 'o', "",
                    "Output directory to rename/remux the file.");
  _parser.setOption("blacklist", 'b', "", "Blacklist file containing filters");
}

int Rename::run() {
  std::filesystem::path outputDir;
  std::filesystem::path workingFile(_filename);
  try {
    _parser.parse();

    if (_parser.isSetOption("help")) {
      std::cout << _parser << std::endl;
      return 0;
    }

    if (!std::filesystem::is_regular_file(workingFile)) {
      throw std::runtime_error(
          fmt::format("{} is not a regular file.", _filename));
    }

    outputDir = std::filesystem::absolute(workingFile).parent_path();
    std::cerr << "outputdir" << outputDir.string() << std::endl;
    if (_parser.isSetOption("output-directory")) {
      outputDir = _parser.getOption<std::string>("output-directory");
    }

    if (!std::filesystem::is_directory(outputDir)) {
      throw std::runtime_error(
          fmt::format("{} is not a directory", outputDir.string()));
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  Explorer::Engine engine;
  try {
    std::string key = _parser.getOption<std::string>("api_key");
    engine.setTmdbKey(key);
  } catch (const std::exception& e) {
    std::cerr << "You need to provide an API key" << std::endl;
    return 1;
  }
  engine.setLanguage(_parser.getOption<std::string>("language"));

  if (_parser.isSetOption("blacklist")) {
    engine.setBlacklist(_parser.getOption<std::string>("blacklist"));
  }
  try {
    auto prediction = engine.predictFile(_filename);
    prediction.input.dumpInfo();
    if (prediction.movie) {
      fmt::print("{} ({})\n", prediction.movie->title,
                 prediction.movie->release_date);
      fmt::print("{}\n", prediction.movie->overview);
      fmt::print("Rating: {:2.0f}%\n", prediction.movie->vote_average * 10);
      fmt::print("Output file => {}\n", prediction.output);
    } else if (prediction.tvshow && prediction.episode) {
      fmt::print("{} {:d}x{:02d} {} ({})\n", prediction.tvshow->name,
                 prediction.episode->season_number,
                 prediction.episode->episode_number, prediction.episode->name,
                 prediction.episode->air_date);
      fmt::print("{}\n", prediction.episode->overview);
      fmt::print("Rating: {:2.0f}%\n", prediction.episode->vote_average * 10);
      fmt::print("Output file => {}\n", prediction.output);
    } else {
      std::cout << prediction.tvshow.get() << std::endl;
      std::cout << prediction.episode.get() << std::endl;
      return 1;
    }
    if (_parser.isSetOption("dry-run"))
      return 0;

    Media::FileInfo::Container container = Media::FileInfo::Container::Mkv;
    if (_parser.getOption<std::string>("muxer") == "mp4")
      container = Media::FileInfo::Container::Mp4;

    return engine.apply(prediction, container, outputDir);
  } catch (const std::exception& e) {
    std::cerr << "Failed to predict file " << _filename << std::endl;
    return 1;
  }
}

} // namespace Cli

} // namespace TitleFinder

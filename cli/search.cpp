/**
 * @file cli/search.cpp
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

#include "search.hpp"

#include <fmt/ostream.h>
#include <iostream>

#include "explorer/engine.hpp"

namespace TitleFinder {

namespace Cli {

Search::Search(int argc, char* argv[]) : SubApp(argc, argv), _query() {
  _parser.setBinaryName(TITLEFINDER_NAME " search");
  _parser.setOption("type", 't', "movie", "Type of research",
                    {"movie", "tvshow"});
  _parser.setOption("season", 's', "", "Season number");
  _parser.setOption("seasons", 'S', "Print all seasons");
  try {
    _parser.parse();
  } catch (const std::exception& e) {
    std::cerr << e.what();
  }
  if (argc - 1 >= 0)
    _query = argv[argc - 1];
}

int Search::run() {
  if (_columns > 120)
    _columns = 120;

  try {
    if (_parser.isSetOption("help")) {
      std::cout << _parser << std::endl;
      return 0;
    }

    if (_parser.isSetOption("seasons") && _parser.isSetOption("season")) {
      std::cerr << "Options \"seasons\" and \"season\" are exclusive."
                << std::endl;
      return 1;
    }

    if (_parser.isSetOption("season")) {
      _parser.getOption<int>("season");
    }
  } catch (const std::exception& e) {
    fmt::print(std::cerr, "Exception occured: {}\n", e.what());
    return 1;
  }

  SubApp::readyEngine();

  std::string type = _parser.getOption<std::string>("type");
  try {
    if (type == "movie") {
      auto result = _engine.searchMovie(_query);
      if (result->total_results == 0) {
        std::cout << "No match found." << std::endl;
        return 1;
      }
      std::cout << fmt::format("|{: ^{}s}|{: ^4s}|{: ^6s}|", "Title",
                               _columns - 14, "Year", "Rating")
                << std::endl;
      for (auto& f : result->results) {
        std::cout << fmt::format("|{: <{}s}|{: ^4s}|{: >5.0f}%|", f.title,
                                 _columns - 14, f.release_date.substr(0, 4),
                                 f.vote_average * 10)
                  << std::endl;
      }
    } else {
      auto result = _engine.searchTvShow(_query);
      if (result->total_results == 0) {
        std::cout << "No match found." << std::endl;
        return -1;
      }
      if (!_parser.isSetOption("seasons") && !_parser.isSetOption("season")) {
        std::cout << fmt::format("|{: ^{}s}|{: ^4s}|{: ^6s}|", "Name",
                                 _columns - 14, "Year", "Rating")
                  << std::endl;
        for (auto& f : result->results) {
          std::cout << fmt::format("|{: <{}s}|{: >4s}|{: >5.0f}%|", f.name,
                                   _columns - 14, f.first_air_date.substr(0, 4),
                                   f.vote_average * 10)
                    << std::endl;
        }
      } else {
        auto showId = result->results.front().id;
        auto details = _engine.getTvShowDetails(showId);
        auto info = fmt::format("{} ({} seasons)", details->name,
                                details->number_of_seasons);
        std::cout << fmt::format("|{: ^{}s}|{: ^10s}|{: ^6s}|", info,
                                 _columns - 20, "Date", "Rating")
                  << std::endl;
        int only = -1;
        if (_parser.isSetOption("season"))
          only = _parser.getOption<int>("season");
        for (int si = 1; si <= details->number_of_seasons; ++si) {
          if (only > 0 && si != only)
            continue;
          auto season = _engine.getSeasonDetails(showId, si);
          std::cout << fmt::format("|{: ^{}s}|{: ^10s}|{: ^6s}|", season->name,
                                   _columns - 20, season->air_date, "")
                    << std::endl;
          for (auto& ep : season->episodes) {
            std::cout << fmt::format(
                             "|{: >2d}x{:0>2d} {: <{}s}|{: ^10s}|{: >5.0f}%|",
                             ep.season_number, ep.episode_number, ep.name,
                             _columns - 26, ep.air_date, ep.vote_average * 10)
                      << std::endl;
          }
        }
      }
    }
  } catch (const std::exception& e) {
    fmt::print(std::cerr, "Exception occured: {}", e.what());
  }
  return 0;
}

} // namespace Cli

} // namespace TitleFinder

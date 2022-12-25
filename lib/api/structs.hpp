/**
 * @file api/structs.hpp
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

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace TitleFinder {

namespace Api {

#define fillOption(json__, option__)                                           \
  if (json__.contains(#option__) && !json__[#option__].is_null())              \
    option__ = json__.value(#option__, option__);

struct SearchInfo {
  std::string poster_path{};
  std::string overview{};
  std::vector<int> genre_ids{};
  int id{-1};
  std::string original_language{};
  std::string backdrop_path{};
  double popularity{-1};
  int vote_count{-1};
  double vote_average{-1};
  inline void from_json(const nlohmann::json& j) {
    fillOption(j, poster_path);
    fillOption(j, overview);
    fillOption(j, poster_path);
    fillOption(j, id);
    fillOption(j, original_language);
    fillOption(j, backdrop_path);
    fillOption(j, popularity);
    fillOption(j, vote_count);
    fillOption(j, vote_average);
    if (j.contains("genre_ids")) {
      j["genre_ids"].get_to(genre_ids);
    }
  }
};

struct MovieInfoCompact : SearchInfo {
  bool adult{false};
  std::string release_date{"0000-00-00"};
  std::string original_title{"No title"};
  std::string title{};
  bool video{false};
  inline void from_json(const nlohmann::json& j) {
    SearchInfo::from_json(j);
    fillOption(j, adult);
    fillOption(j, release_date);
    fillOption(j, original_title);
    fillOption(j, title);
    fillOption(j, video);
  }
};

struct TvShowInfoCompact : SearchInfo {
  std::string first_air_date{"0000-00-00"};
  std::vector<std::string> origin_country{};
  std::string name{"No Name"};
  std::string original_name{"No Name"};
  inline void from_json(const nlohmann::json& j) {
    SearchInfo::from_json(j);
    fillOption(j, first_air_date);
    fillOption(j, name);
    fillOption(j, original_name);
    if (j.contains("origin_country")) {
      j["origin_country"].get_to(origin_country);
    }
  }
};

struct Episode {
  std::string air_date{"0000-00-00"};
  int episode_number{-1};
  int id{-1};
  std::string name{"No Name"};
  std::string overview{};
  std::string production_code{};
  int season_number{-1};
  std::string still_path{};
  double vote_average{0};
  int vote_count{0};

  inline void from_json(const nlohmann::json& j) {
    fillOption(j, air_date);
    fillOption(j, episode_number);
    fillOption(j, id);
    fillOption(j, name);
    fillOption(j, overview);
    fillOption(j, production_code);
    fillOption(j, season_number);
    fillOption(j, still_path);
    fillOption(j, vote_average);
    fillOption(j, vote_count);
  }
};

} // namespace Api

} // namespace TitleFinder
